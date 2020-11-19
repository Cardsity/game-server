#pragma once
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
        std::string text;
    };
    struct BlackCard
    {
        std::string text;
        std::uint8_t blanks;
    };

    struct Hand
    {
        std::vector<WhiteCard> whiteCards;
        std::vector<BlackCard> blackCards;
    };

    struct Player
    {
        std::string name;
        std::uint16_t id;
        std::string color;

        Hand hand;
        bool isCzar;
        std::uint16_t points;
        std::uint8_t jokerRequests;
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

    class_(Hand).property(&Hand::whiteCards, "whiteCards").property(&Hand::blackCards, "blackCards");

    // Intentionally left out hand, it will only be sent to the cliet via HandUpdate
    class_(Player)
        .property(&Player::name, "name")
        .property(&Player::id, "id")
        .property(&Player::color, "color")
        .property(&Player::isCzar, "isCzar")
        .property(&Player::points, "points")
        .property(&Player::jokerRequests, "jokerRequests");
}
FINISH