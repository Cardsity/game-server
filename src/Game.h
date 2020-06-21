#pragma once
#include <string>
#include <atomic>
#include <future>
#include "Requests.h"
#include "safe_ptr.h"
#include <functional>
#include "Connection.h"
#include <websocketpp/server.hpp>

using uint = unsigned int;
using handle = websocketpp::connection_hdl;

struct WhiteCard
{
	std::string text;
	uint ownerId = 0;

	bool operator==(std::string o)
	{
		return text == o;
	}
};
struct BlackCard
{
	std::string text;
	uint blanks = 1;
};
struct Deck
{
	std::vector<WhiteCard> whiteCards;
	std::vector<BlackCard> blackCards;
	std::string name;
	uint id;

	bool operator==(const std::string& o)
	{
		return o == std::to_string(id);
	}
	bool operator==(const Deck& o)
	{
		return o.id == this->id;
	}
};

struct Player
{
	Connection owner;
	
	uint points = 0;
	uint jokerRequests; //SERIALIZEPRIV

	std::vector<WhiteCard> hand; //SERIALIZEPRIV
	std::vector<WhiteCard> playedCards; //SERIALIZEPRIV

	bool ownsCards(std::vector<std::string>);
	bool ownsCard(std::string);
	void playCard(std::string);
	void autoPlay(uint amount);
	void addCard(std::string);

	bool operator==(Player& o)
	{
		return o.owner == this->owner;
	}
	bool operator!=(Player& o)
	{
		return o.owner != this->owner;
	}
	bool operator==(const Connection& o)
	{
		return this->owner == o;
	}
	bool operator!=(const Connection& o)
	{
		return this->owner != o;
	}
	bool operator==(const uint& o)
	{
		return this->owner == o;
	}
	bool isValid()
	{
		return owner.loggedIn && owner.name.length() > 0;
	}
	Player() {}
	Player(const Connection& o)
	{
		this->owner = o;
	}
};

struct Lobby
{
	uint id;
	std::string name;
	std::atomic<bool> isValid = true;

	/*Settings*/
	bool winnerBecomesCzar;
	bool jokerCardsToDeck;
	uint maxJokerRequests;
	std::string password;
	float pickLimit;
	uint maxPlayers;
	uint maxRounds;
	uint maxPoints;
	/*        */
	
	Player czar;
	Player lastCzar;
	Player lastWinner;
	std::atomic<uint> czarPicked = 0;

	BlackCard blackCard;
	std::atomic<uint> currentRound = 0;
	
	sf::safe_ptr<std::vector<Player>> livePlayers;
	sf::safe_ptr<std::vector<Deck>> decks;
	
	Status sendChatMessage(Connection, SendChatMessageRequest);
	
	Status czarPick(Connection, CzarPickRequest);
	Status playCards(Connection, CardPlayRequest);
	Status jokerRequest(Connection, JokerCardRequest);
	
	Status updateSettings(Connection, UpdateGameRequest);
	Status kickPlayer(Connection, KickRequest);
	Status startGame(Connection);

	void forceConnectAndCreate(Connection, CreateGameRequest, std::string requestId);
	Status connect(Connection, JoinGameRequest);
	Status disconnect(Connection);
	
	void addDeck(std::string id);
	void runGameAsync(); //This runs async and only accesses the safePlayerCpy and the playerChangeStack - aswell as currentRound, czar and blackCard - as it is the only thread that
	//writes to those its not a problem as they can be read asynchronous.

	Lobby() {}
	Lobby(const Lobby& o)
	{
		this->id = o.id;
		this->name = o.name;
		this->password = o.password;
		this->isValid = o.isValid.load();

		this->pickLimit = o.pickLimit;
		this->maxRounds = o.maxRounds;
		this->maxPoints = o.maxPoints;
		this->maxPlayers = o.maxPlayers;
		this->maxJokerRequests = o.maxJokerRequests;

		this->czar = o.czar;
		this->blackCard = o.blackCard;
		this->livePlayers->insert(livePlayers->end(), o.livePlayers->begin(), o.livePlayers->end());
		this->czarPicked = o.czarPicked.load();
		this->currentRound = o.currentRound.load();
		this->decks->insert(decks->end(), o.decks->begin(), o.decks->end());
	}
private:
	std::atomic<bool> isIngame = false;
	std::vector<std::future<void>> futures;
	std::atomic<bool> shouldCzarPick = false;
	std::atomic<bool> shouldPlayerPlay = false;
	sf::safe_ptr<std::vector<Player>> safePlayerCpy;
	sf::safe_ptr<std::vector<std::tuple<Connection, BlackCard, std::vector<WhiteCard>>>> wonCards;
	sf::safe_ptr<std::vector<std::function<void(std::vector<Player>::iterator)>>> playerActionStack;
	sf::safe_ptr<std::vector<std::function<void(sf::safe_ptr<std::vector<Player>>&)>>> playerChangeStack;
	void sendGameUpdate();
};

inline sf::safe_ptr<std::map<uint, Lobby>> lobbies;