#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include "../server.h"
#include "../utils/reflection.h"

namespace Cardsity::GameObjects
{
    struct GameSettings
    {
        std::uint8_t maxPlayers;
        std::uint8_t maxRounds;
        std::uint8_t maxPoints;
        std::uint8_t maxJokers;
        std::uint8_t pickLimit;

        std::string password;
        std::vector<std::string> decks;

        bool winnerCzar;
        bool inviteOnly;
        bool jokerToDeck;
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

    struct Player
    {
        std::string name;
        std::uint16_t id;
        std::string color;

        std::uint8_t points;
        std::uint8_t jokerRequests;
        std::map<std::uint32_t, WhiteCard> hand;

        bool operator==(const Player &other)
        {
            return other.id == id;
        }
        bool operator!=(const Player &other)
        {
            return other.id != id;
        }
    };

    struct CardStack
    {
        Player owner;
        std::vector<WhiteCard> cards;
    };

    struct GameState
    {
        Player czar;
        BlackCard blackCard;
        std::atomic<std::uint8_t> round;
        std::vector<CardStack> playedCards;

        bool inGame()
        {
            return round > 0;
        }
    };
    enum InternalGameState : std::uint8_t
    {
        HANDOUT_CARDS,
        WAIT_FOR_PLAYERS,
        WAIT_FOR_CZAR,
        CZAR_PICKED,
    };

    struct Game
    {
        Player host;
        GameState state;
        std::uint16_t id;
        GameSettings settings;
        std::map<std::shared_ptr<Server::WsServer::Connection>, Player> players;

        void onTick(std::uint64_t);

        void kick(std::shared_ptr<Server::WsServer::Connection>, std::uint16_t);
        void onChatMessage(std::shared_ptr<Server::WsServer::Connection>, const std::string &);

        void onPickWinner(std::shared_ptr<Server::WsServer::Connection>, std::uint8_t);
        void onPlayCards(std::shared_ptr<Server::WsServer::Connection>, std::vector<std::uint32_t>);

        void onDisconnect(std::shared_ptr<Server::WsServer::Connection>);
        void onConnect(std::shared_ptr<Server::WsServer::Connection>, Player);

      private:
        std::mutex playersMutex;
        std::mutex playedCardsMutex;
        std::mutex playerStatesMutex;

        std::vector<WhiteCard> whiteCardPool;
        std::vector<BlackCard> blackCardPool;

        std::vector<Player> playerStates;
        std::atomic<std::uint8_t> internalState;
        std::map<std::uint8_t, std::reference_wrapper<Player>> concealedPlayers;

        bool waitForTick;
        std::uint64_t nextTick;
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

    // Intentionally left out hand & jokerRequests, it will only be sent to the client via HandUpdate
    class_(Player)
        .property(&Player::id, "id")
        .property(&Player::name, "name")
        .property(&Player::color, "color")
        .property(&Player::points, "points");

    class_(GameState)
        .property(&GameState::blackCard, "blackCard")
        .property(&GameState::czar, "czar")
        .property(&GameState::round, "round");

    class_(Game)
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