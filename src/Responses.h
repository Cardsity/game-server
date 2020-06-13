#pragma once
#include "Game.h"

struct Welcome
{
	Connection you;
};
struct UnrevealedCard
{
	uint unrevealedCardOwnerId;
};
struct RevealCards
{
	std::vector<WhiteCard> cards;
};
struct CzarPickNotify
{
	uint winnerId;
};
struct SentChatMessage
{
	Connection sender;
	std::string message;
};
struct HandUpdate
{
	std::vector<WhiteCard> newHand;
	bool isCausedByJokerRequest;
	uint jokerRequestsRemaining;
};
struct GameEnd
{
	Player winner;
	std::vector<std::tuple<Connection, BlackCard, std::vector<WhiteCard>>> cardHistory;

	GameEnd() {}
	GameEnd(Player winner, sf::safe_ptr<std::vector<std::tuple<Connection, BlackCard, std::vector<WhiteCard>>>> c)
	{
		this->winner = winner;
		this->cardHistory.insert(cardHistory.end(), c->begin(), c->end());
	}
};
struct KickNotify
{
	std::string kickReason;
};
struct DeckInfo
{
	uint id;
	std::string name;
	DeckInfo() {}
	DeckInfo(const Deck& d)
	{
		this->name = d.name;
		this->id = d.id;
	}
	DeckInfo operator=(const Deck& o)
	{
		DeckInfo d;
		d.name = o.name;
		d.id = o.id;
		return d;
	}
};
struct LobbyStatus
{
	std::vector<Player> players;
	std::vector<DeckInfo> decks;
	std::string password;
	BlackCard blackCard;
	uint currentRound;
	std::string name;
	Player czar;
	uint id;

	uint maxJokerRequests;
	float pickLimit;
	uint maxPlayers;
	uint maxRounds;
	uint maxPoints;

	LobbyStatus() {}
	LobbyStatus(const Lobby& l);
};
struct LobbyInfo
{
	uint id;
	bool isValid; //SERIALIZEPRIV
	bool password;
	uint maxPlayers;
	std::string name;
	uint playerCount;
	std::string hostName;

	LobbyInfo() {}
	LobbyInfo(const Lobby& l);
};
struct LobbyListResponse
{
	std::vector<LobbyInfo> lobbies;
	uint totalRows;
};