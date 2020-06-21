#include "Json.h"
#include "Game.h"
#include "Logs.h"
#include "Utils.h"
#include "Server.h"
#include <cpr/cpr.h>
#include "Responses.h"
#include "serialize.h"
#include "Validation.h"

void Lobby::sendGameUpdate()
{
	linfo("ID ", this->id, ": Sending GameUpdates");
	LobbyStatus packet(*this);

	std::vector<Player> safeLivePlayers(livePlayers->begin(), livePlayers->end());
	for (auto player : safeLivePlayers)
	{
		server.send(player.owner.hdl, packet, "", id);
	}
}
Status Lobby::sendChatMessage(Connection con, SendChatMessageRequest message)
{
	linfo("ID ", this->id, ": Received Chat Message");
	if (!isValid)
		return Status{ "Not valid", false };
	if (!messageValidation(message.message))
		return Status{ "Invalid message", false };
	if (!sfLContains<Player>(livePlayers, con))
		return Status{ "Invalid Lobby", false };

	SentChatMessage packet{ con, message.message };
	foreach(player, livePlayers)
	{
		server.send(player->owner.hdl, packet, "", id);
	}

	return Status{ "Message sent", true };
}
Status Lobby::jokerRequest(Connection con, JokerCardRequest request)
{
	linfo("ID ", this->id, ": Received Joker Request");
	if (!isValid)
		return Status{ "Not valid", false };
	if (!isIngame)
		return Status{ "Not ingame", false };
	if (!messageValidation(request.text))
		return Status{ "Invalid text", false };

	auto player = sfgetItemByValue<Player, std::vector<Player>>(safePlayerCpy, con);
	if (!player.has_value())
		return Status{ "Invalid player", false };
	if ((*player)->jokerRequests <= 0)
		return Status{ "No JokerCards left", false };

	this->playerActionStack->push_back([con, request, this](std::vector<Player>::iterator p)
	{
		if (*p == con && p->jokerRequests > 0)
		{
			p->addCard(request.text);
			p->jokerRequests--;

			if (jokerCardsToDeck)
			{
				decks->front().whiteCards.push_back(WhiteCard{ request.text });
			}

			server.send(con.hdl, HandUpdate{ p->hand, true, p->jokerRequests }, "", this->id);
		}
	});

	return Status{ "JokerCard granted!", true };
}
Status Lobby::czarPick(Connection con, CzarPickRequest request)
{
	linfo("ID ", this->id, ": Received Czar Pick");
	if (!isValid)
		return Status{ "Not valid", false };
	if (!isIngame)
		return Status{ "Not ingame", false };
	if (!shouldCzarPick)
		return Status{ "You can't pick right now!", false };
	if (czarPicked != 0)
		return Status{ "Already picked!", false };
	if ((Player)czar != con)
		return Status{ "You're not czar", false };

	auto player = sfgetItemByValue<Player, std::vector<Player>>(livePlayers, request.winnerId);
	if (!player.has_value())
		return Status{ "Player not found!", false };
	if ((Player)czar == **player)
		return Status{ "Invalid player!", false };

	czarPicked = request.winnerId;
	return Status{ "Sucessfully picked!", true };
}
Status Lobby::playCards(Connection con, CardPlayRequest request)
{
	linfo("ID ", this->id, ": Received CardPlayRequest");
	if (!isValid)
		return Status{ "Not valid", false };
	if (!isIngame)
		return Status{ "Not ingame", false };

	auto player = sfgetItemByValue<Player, std::vector<Player>>(safePlayerCpy, con);

	if (!player.has_value())
		return Status{ "Invalid Lobby", false };
	if (!shouldPlayerPlay)
		return Status{ "You can't play right now!", false };
	if ((Player)czar == con)
		return Status{ "You can't play as czar!", false };
	if (request.cards.size() != blackCard.blanks)
		return Status{ "Invalid card amount!", false };
	if (!(*player)->ownsCards(request.cards))
		return Status{ "You don't own one or more of those cards", false };
	if ((*player)->playedCards.size() > 0)
		return Status{ "You already played!", false };

	this->playerActionStack->push_back([con, request](std::vector<Player>::iterator p)
	{
		if (*p == con && p->playedCards.size() == 0)
		{
			for (auto card : request.cards)
				p->playCard(card);
		}
	});

	foreach(player, livePlayers)
	{
		for (auto card : request.cards)
		{
			UnrevealedCard uCard{ con.id };
			server.send(player->owner.hdl, uCard, "", id);
		}
	}

	return Status{ "Sucessfully played cards!", true };
}
Status Lobby::kickPlayer(Connection con, KickRequest request)
{
	linfo("ID ", this->id, ": Received KickPlayer Request");
	if (!isValid)
		return Status{ "Not valid", false };
	if (!sfLContains<Player>(livePlayers, con))
		return Status{ "Invalid Lobby", false };
	if (livePlayers->size() <= 0)
		return Status{ "Wtf?", false };
	if (livePlayers->at(0) != con)
		return Status{ "You're not host!", false };
	if (!messageValidation(request.message))
		return Status{ "Invalid kick message!", false };

	if (auto pIt = sfgetItemByValue<Player, std::vector<Player>>(livePlayers, request.playerId); pIt.has_value())
		livePlayers->erase(*pIt);

	this->playerChangeStack->push_back([request](sf::safe_ptr<std::vector<Player>>& list)
	{
		auto pIt = sfgetItemByValue<Player, std::vector<Player>>(list, request.playerId);
		if (pIt.has_value())
			list->erase(*pIt);
	});

	sendGameUpdate();
	return Status{ "Sucessfully kicked!", true };
}
Status Lobby::startGame(Connection con)
{
	linfo("ID ", this->id, ": Received Start Game Request");
	if (!isValid)
		return Status{ "Not valid", false };
	if (isIngame)
		return Status{ "Already ingame", false };
	if (!sfLContains<Player>(livePlayers, con))
		return Status{ "Invalid Lobby", false };
	if (livePlayers->size() <= 0)
		return Status{ "Wtf?", false };
	if (livePlayers->at(0) != con)
		return Status{ "You're not host!", false };
	if (decks->size() < 1)
		return Status{ "Too few decks", false };
	if (decks->size() > 10)
		return Status{ "Too many decks", false };

	this->runGameAsync();
	return Status{ "Starting game!", true };
}
Status Lobby::connect(Connection con, JoinGameRequest request)
{
	linfo("ID ", this->id, ": Received Connect Request");
	if (request.lobbyId != this->id)
		return Status{ "Invaid lobby", false };
	if (!this->isValid)
		return Status{ "Invalid lobby", false };
	if (request.password.size() > 0 && !passwordValidation(request.password))
		return Status{ "Invalid password", false };
	if (this->password.size() > 0 && this->password != request.password)
		return Status{ "Invalid password", false };
	if (this->isIngame)
		return Status{ "Can't join while game is in progress", false };
	if (this->livePlayers->size() >= maxPlayers)
		return Status{ "Lobby is full", false };

	livePlayers->push_back(con);
	playerChangeStack->push_back([con](sf::safe_ptr<std::vector<Player>>& list)
	{
		list->push_back(con);
	});
	sendGameUpdate();

	return Status{ "Sucessfully connected!", true };
}
void Lobby::forceConnectAndCreate(Connection con, CreateGameRequest request, std::string requestId)
{
	futures.push_back(std::async(std::launch::async, [&](std::string requestId, CreateGameRequest request, Connection con)
	{
		linfo("ID ", this->id, ": A player is being force connected");
		livePlayers->push_back(con);
		safePlayerCpy->push_back(con);

		for (auto deck : request.decks)
		{
			addDeck(deck);
		}

		server.send(con.hdl, Status{ "Sucessfully connected!", true });
		server.send(con.hdl, LobbyStatus(*this), requestId);
		sendGameUpdate();
	}, requestId, request, con));
}
Status Lobby::disconnect(Connection con)
{
	linfo("ID ", this->id, ": A player is being disconnected");
	auto pIt = sfgetItemByValue<Player, std::vector<Player>>(livePlayers, con);
	if (!pIt.has_value())
		return Status{ "You're not in this lobby!", false };

	livePlayers->erase(*pIt);
	this->playerChangeStack->push_back([con](sf::safe_ptr<std::vector<Player>>& list)
	{
		auto pIt = sfgetItemByValue<Player, std::vector<Player>>(list, con);
		if (pIt.has_value())
			list->erase(*pIt);
	});

	if (livePlayers->size() <= 0)
	{
		isValid = false;

		if (!this->isIngame)
		{
			if (lobbies->find(this->id) != lobbies->end())
				lobbies->erase(this->id);
		}
		else
		{
			futures.push_back(std::async(std::launch::async, [&]
			{
				while (this->isIngame)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				if (lobbies->find(this->id) != lobbies->end())
					lobbies->erase(this->id);
			}));
		}
	}
	else
	{
		sendGameUpdate();
	}

	return Status{ "Disconnected from lobby", true };
}
Status Lobby::updateSettings(Connection con, UpdateGameRequest request)
{
	if (isIngame)
		return Status{ "Can't update settings right now", false };
	if (!isValid)
		return Status{ "Invalid lobby", false };
	if (livePlayers->size() <= 0)
		return Status{ "Wtf?", false };
	if (livePlayers->at(0) != con)
		return Status{ "You're not host!", false };
	if (
		request.maxPlayers > 15 || request.maxPlayers < 3
		|| request.maxPoints > 25 || request.maxPoints < 5
		|| request.maxRounds > 20 || request.maxRounds < 5
		|| request.pickLimit > 5 || request.pickLimit < 0.5
		|| request.maxJokerRequests < 0 || request.maxJokerRequests > 10
		|| request.decks.size() > 10 || request.decks.size() < 1
		)
		return Status{ "Invalid game settings!", false };

	this->maxJokerRequests = request.maxJokerRequests;
	this->maxPlayers = request.maxPlayers;
	this->maxRounds = request.maxRounds;
	this->pickLimit = request.pickLimit * 60'000;
	this->maxPoints = request.maxPoints;
	this->password = request.password;

	futures.push_back(std::async(std::launch::async, [&](UpdateGameRequest request)
	{
		decks->erase(std::remove_if(decks->begin(), decks->end(), [request](Deck& d)
		{
			if (!LContains<std::string>(request.decks, std::to_string(d.id)))
			{
				return true;
			}
			return false;
		}), decks->end());

		for (auto deck : request.decks)
		{
			if (!sfLContains<Deck>(decks, deck))
			{
				addDeck(deck);
			}
		}
		sendGameUpdate();
	}, request));

	return Status{ "Updated!", true };
}
void Lobby::addDeck(std::string id)
{
	if (this->decks->size() >= 10) return;
	if (sfLContains<Deck>(decks, id)) return;

	linfo(this->id, ": Adding Deck ", id);
	auto res = cpr::Get(
		cpr::Url{ "http://cds:8020/deck/" + id + "/json" }
	);
	if (res.status_code == 200)
	{
		linfo(this->id, ": Deck ", id, " returned 200 OK!");
		auto js = UnsafeJson::getUnsafeJson(res.text);
		if (js.valid)
		{
			linfo(this->id, ": Deck ", id, " json is valid!");
			auto& j = js.json;

			Deck deck;
			deck.id = j["id"].get<uint>();
			deck.name = j["name"].get<std::string>();
			deck.whiteCards = j["white_cards"].get<std::vector<WhiteCard>>();
			deck.blackCards = j["black_cards"].get<std::vector<BlackCard>>();
			decks->push_back(deck);
			sendGameUpdate();
		}
		else
		{
			lerror(this->id, ": Deck ", id, " json is invalid!");
		}
	}
	else
	{
		lerror(this->id, ": Deck fetching failed: ", id, " fetch returned: ", res.status_code, " raw: ", res.text);
	}

	linfo(this->id, ": Deck fetching for ", id, " finished!");
}

void Lobby::runGameAsync()
{
	isIngame = true;
	linfo("ID ", this->id, ": Let the games begin!");
	futures.push_back(std::async(std::launch::async, ([&]
	{
		linfo("ID ", this->id, ": Starting new game!");
		foreach(action, playerChangeStack)
		{
			(*action)(safePlayerCpy);
		}
		playerChangeStack->clear();

		linfo("ID ", this->id, ": Giving players random cards and resetting their stats!");
		foreach(player, safePlayerCpy)
		{
			player->points = 0;
			player->hand.clear();
			player->playedCards.clear();
			player->jokerRequests = maxJokerRequests;
			for (int i = 0; 10 > i; i++)
			{
				auto randomDeck = sfgetRandomFromList<Deck>(decks);
				auto whiteCard = getRandomFromList(randomDeck->whiteCards);
				player->addCard(whiteCard->text);
			}
		}

		while (isIngame && isValid)
		{
			if (currentRound >= maxRounds) break;

			linfo("ID ", this->id, ": A new round has begun");

			currentRound++;
			foreach(action, playerChangeStack)
			{
				(*action)(safePlayerCpy);
			}
			playerChangeStack->clear();
			if (safePlayerCpy->size() < 3)
				break;

			linfo("ID ", this->id, ": Setting Czar and Blackcard");
			czarPicked = 0;
			blackCard = *getRandomFromList(sfgetRandomFromList<Deck>(decks)->blackCards);

			if (winnerBecomesCzar && lastWinner.isValid())
			{
				czar = lastWinner;
				lastCzar = lastWinner;
			}
			else
			{
				czar = *sfgetRandomFromList<Player>(safePlayerCpy);
				while (czar == lastCzar)
				{
					czar = *sfgetRandomFromList<Player>(safePlayerCpy);
				}
				lastCzar = czar;
			}

			linfo("ID ", this->id, ": Giving each player ", blackCard.blanks, " Cards");
			foreach(player, safePlayerCpy)
			{
				player->playedCards.clear();
				if (*player != czar)
				{
					for (int i = 0; blackCard.blanks > i; i++)
					{
						auto randomDeck = sfgetRandomFromList<Deck>(decks);
						auto whiteCard = getRandomFromList(randomDeck->whiteCards);
						player->addCard(whiteCard->text);
					}
				}
				HandUpdate newHand{ player->hand, false, player->jokerRequests };
				server.send(player->owner.hdl, newHand, "", id);
			}
			sendGameUpdate();

			linfo("ID ", this->id, ": Waiting for players to play");
			auto start = std::chrono::high_resolution_clock::now();
			shouldPlayerPlay = true;
			while (!hasTimePassed(start, this->pickLimit))
			{
				foreach(change, playerActionStack)
				{
					foreach(player, safePlayerCpy)
					{
						ldebug("ID ", this->id, ": Fetching Player Actions");
						(*change)(player);
					}
				}
				playerActionStack->clear();
				foreach(action, playerChangeStack)
				{
					ldebug("ID ", this->id, ": Fetching Player Changes");
					(*action)(safePlayerCpy);
					sendGameUpdate();
				}
				playerChangeStack->clear();

				uint totalPicked = 0;
				foreach(player, safePlayerCpy)
				{
					totalPicked += player->playedCards.size();
				}

				const uint expectedToPick = sfLContains<Player>(safePlayerCpy, czar) ? safePlayerCpy->size() - 1 : safePlayerCpy->size();
				if (totalPicked >= blackCard.blanks * expectedToPick)
				{
					linfo("ID ", this->id, ": Everyone picked!");
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			shouldPlayerPlay = false;

			foreach(player, safePlayerCpy)
			{
				if (czar != *player && player->playedCards.size() != blackCard.blanks)
				{
					linfo("ID ", this->id, ": Auto-Playing for: ", player->owner.name);
					player->autoPlay(blackCard.blanks);
				}
			}

			linfo("ID ", this->id, ": Revealing Cards");
			RevealCards reveal;
			for (auto player : sfShuffle(safePlayerCpy))
			{
				reveal.cards.insert(reveal.cards.end(), player.playedCards.begin(), player.playedCards.end());
			}
			foreach(player, safePlayerCpy)
			{
				server.send(player->owner.hdl, reveal, "", id);
			}

			linfo("ID ", this->id, ": Waiting for czar to pick");
			start = std::chrono::high_resolution_clock::now();
			shouldCzarPick = true;
			while (!hasTimePassed(start, pickLimit))
			{
				foreach(action, playerChangeStack)
				{
					(*action)(safePlayerCpy);
					sendGameUpdate();
				}
				playerChangeStack->clear();

				if (!sfLContains<Player>(safePlayerCpy, czar))
					break;
				if (czarPicked != 0)
					break;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			shouldCzarPick = false;
			while ((czarPicked == 0 || czarPicked == czar.owner.id) && livePlayers->size() > 0)
			{
				auto randomPlayer = sfgetRandomFromList<Player>(safePlayerCpy);
				if (!randomPlayer.has_value())
					break;
				czarPicked = randomPlayer->owner.id;
			}

			auto winner = sfgetItemByValue<Player, std::vector<Player>>(safePlayerCpy, czarPicked);
			if (winner.has_value())
			{
				lastWinner = **winner;
				(*winner)->points += blackCard.blanks;
				wonCards->push_back(std::make_tuple((*winner)->owner, blackCard, (*winner)->playedCards));
			}

			CzarPickNotify notify{ czarPicked };
			foreach(player, safePlayerCpy)
			{
				server.send(player->owner.hdl, notify, "", id);
			}
			foreach(player, safePlayerCpy)
			{
				foreach(p, livePlayers)
				{
					if (*player == *p)
					{
						p->points = player->points;
					}
				}
			}
			sendGameUpdate();

			std::this_thread::sleep_for(std::chrono::seconds(10));

			bool hasWinner = false;
			foreach(player, safePlayerCpy)
			{
				if (player->points >= maxPoints)
				{
					hasWinner = true;
					break;
				}
			}
			if (hasWinner)
			{
				linfo("ID ", this->id, ": We have a winner!");
				break;
			}
		}

		Player winner;
		foreach(player, safePlayerCpy)
		{
			if (player->points > winner.points)
				winner = *player;
		}

		GameEnd packet(winner, wonCards);
		foreach(player, safePlayerCpy)
		{
			server.send(player->owner.hdl, packet, "", id);
		}

		linfo("ID ", this->id, ": Resetting Lobby and Player Stats");
		foreach(player, livePlayers)
		{
			player->points = 0;
		}
		this->currentRound = 0;
		sendGameUpdate();

		playerChangeStack->clear();
		playerActionStack->clear();

		linfo("ID ", this->id, ": Exiting game loop");
		isIngame = false;
		return;
	})));
}