#include "Responses.h"
#include "Requests.h"
#include "Game.h"

LobbyStatus::LobbyStatus(const Lobby& l)
{
	this->players = std::vector<Player>(l.livePlayers->begin(), l.livePlayers->end());
	this->blackCard = l.blackCard;
	this->currentRound = l.currentRound;
	this->name = l.name;
	this->czar = l.czar;
	this->id = l.id;
	this->pickLimit = l.pickLimit;
	this->maxPlayers = l.maxPlayers;
	this->maxRounds = l.maxRounds;
	this->maxPoints = l.maxPoints;
	this->maxJokerRequests = l.maxJokerRequests;
	this->password = l.password;
	this->decks.insert(decks.end(), l.decks->begin(), l.decks->end());
	this->jokerCardsToDeck = l.jokerCardsToDeck;
	this->winnerBecomesCzar = l.winnerBecomesCzar;
}

LobbyInfo::LobbyInfo(const Lobby& l)
{
	this->id = l.id;
	this->name = l.name;
	this->isValid = l.isValid;
	if (l.livePlayers->size() > 0)
		this->hostName = l.livePlayers->at(0).owner.name;
	else
		this->hostName = "undefined";
	this->maxPlayers = l.maxPlayers;
	this->password = l.password.size() > 0;
	this->playerCount = l.livePlayers->size();
}