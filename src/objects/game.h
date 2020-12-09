#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <cstdint>
#include "../server/globals.h"
#include "../utils/reflection.h"

namespace Cardsity
{
    class Server;
}

namespace Cardsity::GameObjects
{
    struct GameSettings
    {
        std::uint8_t maxPlayers;
        std::uint8_t maxRounds;
        std::uint8_t maxPoints;
        std::uint8_t maxJokers;
        std::uint8_t pickLimit; // In seconds!

        std::string password;
        std::vector<std::string> decks;

        bool winnerCzar;
        bool inviteOnly;
        bool jokerToDeck;

        bool valid()
        {
            if (maxPlayers > 15 || maxPlayers < 3)
                return false;
            if (maxPoints > 25 || maxPoints < 5)
                return false;
            if (maxJokers > 10 || maxJokers < 0)
                return false;
            if (decks.size() > 10 || decks.size() < 1)
                return false;
            if (pickLimit > 180 || pickLimit < 30)
                return false;

            return true;
        }
    };

    struct WhiteCard
    {
        std::uint32_t id;
        std::string text;
    };
    struct BlackCard
    {
        std::uint32_t id;
        std::string text;
        std::uint8_t blanks;
    };

    struct Connection
    {
        std::string name;
        std::uint64_t id;
        std::string color;
        std::uint64_t currentLobby;
    };

    struct Player : public Connection
    {
        std::uint8_t points = 0;
        std::uint8_t jokerRequests = 0;
        std::map<std::uint32_t, WhiteCard> hand;

        bool operator==(const Player &other)
        {
            return other.id == id;
        }
        bool operator!=(const Player &other)
        {
            return other.id != id;
        }
        Player()
        {
        }
        Player(const Connection &con)
        {
            id = con.id;
            name = con.name;
            color = con.color;
            currentLobby = con.currentLobby;
        }
    };

    struct CardStack
    {
        Player owner;
        std::vector<WhiteCard> cards;
    };
    struct RoundResult
    {
        Player winner;
        BlackCard blackCard;
        std::vector<CardStack> playedCards;
    };

    struct GameState
    {
        Player czar;
        BlackCard blackCard;
        std::vector<CardStack> playedCards;
        std::atomic<std::uint8_t> round = 0;

        bool inGame()
        {
            return round > 0;
        }
        void operator=(const GameState &other)
        {
            czar = other.czar;
            round = other.round.load();
            blackCard = other.blackCard;
            playedCards = other.playedCards;
        }
    };

    struct Game
    {
        Player host;
        Server &server;
        GameState state;
        std::uint64_t id;
        GameSettings settings;
        std::string lobbyName;
        std::map<con, Player> players;

        void start(con);
        void onTick(std::uint64_t);

        void kick(con, std::uint16_t);
        void onChatMessage(con, const std::string &);

        void onPlayCards(con, std::vector<std::uint32_t>);
        void onPickWinner(con, std::uint8_t, bool = false);

        void onDisconnect(con, bool);
        void onConnect(con, Connection);

        Game(Server &server) : server(server)
        {
        }
        Game(const Game &other) : server(other.server)
        {
            id = other.id;
            host = other.host;
            state = other.state;
            players = other.players;
            settings = other.settings;
            lobbyName = other.lobbyName;
            playerStates = other.playerStates;
            concealedPlayers = other.concealedPlayers;
            internalState = other.internalState.load();

            lastTick = other.lastTick.load();
            nextTick = other.nextTick.load();
            waitForTick = other.waitForTick.load();
        }

      private:
        enum InternalGameState : std::uint8_t
        {
            HANDOUT_CARDS,
            WAIT_FOR_PLAYERS,
            WAIT_FOR_CZAR,
            CZAR_PICKED,
        };

        Player lastWinner;

        std::mutex playersMutex;
        std::mutex gameStateMutex; // Only lock this when we're changing the state! (I don't think there will be any
                                   // problems without this, it's just for sanity...)
        std::mutex concealedMutex;
        std::mutex playedCardsMutex;
        std::mutex playerStatesMutex;

        std::vector<RoundResult> history;
        std::map<std::uint64_t, Player> playerStates;
        std::atomic<std::uint8_t> internalState = HANDOUT_CARDS;
        std::map<std::uint8_t, std::reference_wrapper<Player>> concealedPlayers;

        std::atomic<bool> waitForTick = false;
        std::atomic<std::uint64_t> lastTick = 0;
        std::atomic<std::uint64_t> nextTick = 0;
    };
} // namespace Cardsity::GameObjects

REGISTER
{
    using namespace GameObjects;
    class_(GameSettings)
        .property(&GameSettings::maxPlayers, "maxPlayers")
        .property(&GameSettings::maxRounds, "maxRounds")
        .property(&GameSettings::maxPoints, "maxPoints")
        .property(&GameSettings::maxJokers, "maxJokers")
        .property(&GameSettings::pickLimit, "pickLimit")
        .property(&GameSettings::password, "password")
        .property(&GameSettings::decks, "decks")
        .property(&GameSettings::winnerCzar, "winnerCzar")
        .property(&GameSettings::inviteOnly, "inviteOnly")
        .property(&GameSettings::jokerToDeck, "jokerToDeck");

    class_(WhiteCard).property(&WhiteCard::text, "text");
    class_(BlackCard).property(&BlackCard::text, "text").property(&BlackCard::blanks, "blanks");
    class_(CardStack).property(&CardStack::cards, "cards").property(&CardStack::owner, "owner");
    class_(RoundResult)
        .property(&RoundResult::blackCard, "blackCard")
        .property(&RoundResult::playedCards, "playedCards")
        .property(&RoundResult::winner, "winner");

    // Intentionally left out hand & jokerRequests, it will only be sent to the client via HandUpdate
    class_(Player)
        .property(&Player::id, "id")
        .property(&Player::name, "name")
        .property(&Player::color, "color")
        .property(&Player::points, "points");

    class_(Connection)
        .property(&Connection::id, "id")
        .property(&Connection::name, "name")
        .property(&Connection::color, "color");

    class_(GameState)
        .property(&GameState::blackCard, "blackCard")
        .property(&GameState::czar, "czar")
        .property(
            &GameState::round, "round", [](const std::atomic<std::uint8_t> &round) { return round.load(); }, false);

    class_(Game)
        .property(&Game::lobbyName, "lobbyName")
        .property(&Game::state, "state")
        .property(&Game::host, "host")
        .property(&Game::id, "id")
        .property(&Game::settings, "settings")
        .property(
            &Game::players, "players",
            [](const auto &players) {
                std::vector<Player> rtn;
                for (auto &player : players)
                {
                    rtn.push_back(player.second);
                }
                return rtn;
            },
            false);
}
FINISH