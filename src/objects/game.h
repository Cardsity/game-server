#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <cstdint>
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

        bool isCzar;
        std::uint8_t points;
        std::uint8_t jokerRequests;
        std::map<std::uint32_t, WhiteCard> hand;

        bool operator==(const Player &other)
        {
            return other.id == id;
        }
    };

    struct CardStack //--> Internal Data, not sent
    {
        Player owner;
        std::vector<WhiteCard> cards;
    };
    struct ConcealedCardStack //--> goes to players
    {
        std::uint8_t id; //--> Idea of this is that each player gets assigned a random number when this packet is sent,
                         // so that they will never be able to know who played which cards.
        std::vector<WhiteCard> cards;

        ConcealedCardStack(const CardStack &other)
        {
            cards = other.cards;
            // TODO: Manually set id from game loop
        }
    };

    struct GameState
    {
        std::uint8_t round;
        BlackCard blackCard;
        std::vector<CardStack> playedCards;
    };
    struct Game
    {
        GameState state;
        GameSettings settings;
        std::mutex playersMutex;
        std::vector<Player> players;
        std::vector<WhiteCard> whiteCardPool;
        std::vector<BlackCard> blackCardPool;

        void onTick();
        void kick(Player);
        void onConnect(Player);
        void onDisconnect(Player);
        void onPickWinner(std::uint8_t);
        void onChatMessage(Player, const std::string &);
        void onPlayCards(Player, std::vector<std::uint32_t>);

      private:
        std::uint8_t internalState;
        std::mutex playerStatesMutex;
        std::vector<Player> playerStates;
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

    // Intentionally left out hand & jokerRequests, it will only be sent to the cliet via HandUpdate
    class_(Player)
        .property(&Player::id, "id")
        .property(&Player::name, "name")
        .property(&Player::color, "color")
        .property(&Player::isCzar, "isCzar")
        .property(&Player::points, "points");
}
FINISH