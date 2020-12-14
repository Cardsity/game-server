#pragma once
#include <cstdint>
#include <string>
#include "../../objects/game.h"
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Responses
{
    struct HandUpdate
    {
        std::uint8_t jokerRequests;
        std::map<std::uint32_t, GameObjects::WhiteCard> hand; // Gets converted to vector
    };

    struct ChatMessage // Implemented!
    {
        std::string text;
        GameObjects::Player sender;
    };

    struct PlayerLeave // Implemented!
    {
        GameObjects::Player player;                       // The player who left.
        std::map<rawcon, GameObjects::Player> allPlayers; // Gets converted to vector
    };
    struct PlayerJoin // Implemented!
    {
        GameObjects::Player player;                       // The player who joined.
        std::map<rawcon, GameObjects::Player> allPlayers; // Gets converted to vector
    };
    struct SettingsChange
    {
        GameObjects::GameSettings settings;
    };
    struct GameStateUpdate
    {
        GameObjects::GameState state;
    };
    enum DisconnectReason : std::uint8_t
    {
        LOBBY_CLOSED,
        UNDEFINED,
        KICKED,
    };
    struct Disconnect // Implemented!
    {
        DisconnectReason reason;
    };

    struct PlayedCardsUpdate
    {
        struct ConcealedCardStack
        {
            std::uint8_t owner;
            std::vector<GameObjects::WhiteCard> cards;
        };
        std::vector<ConcealedCardStack> playedCards;
    };

    struct GameEnd // Implemented!
    {
        GameObjects::Player winner;
        std::vector<GameObjects::RoundResult> history;
    };
} // namespace Cardsity::Packets::Responses

REGISTER
{
    using namespace Cardsity::Packets::Responses;
    class_(HandUpdate)
        .property(&HandUpdate::jokerRequests, "jokerRequests")
        .property(&HandUpdate::hand, "hand", [](const std::map<std::uint32_t, WhiteCard> &hand) {
            std::vector<WhiteCard> rtn;
            for (auto &card : hand)
            {
                rtn.push_back(card.second);
            }
            return rtn;
        });
    class_(ChatMessage).property(&ChatMessage::sender, "sender").property(&ChatMessage::text, "text");
    class_(PlayerLeave)
        .property(&PlayerLeave::player, "player")
        .property(
            &PlayerLeave::allPlayers, "allPlayers",
            [](const std::map<rawcon, Player> &players) {
                std::vector<Player> rtn;
                for (auto &player : players)
                {
                    rtn.push_back(player.second);
                }
                return rtn;
            },
            false);
    class_(PlayerJoin)
        .property(&PlayerJoin::player, "player")
        .property(
            &PlayerJoin::allPlayers, "allPlayers",
            [](const std::map<rawcon, Player> &players) {
                std::vector<Player> rtn;
                for (auto &player : players)
                {
                    rtn.push_back(player.second);
                }
                return rtn;
            },
            false);
    class_(SettingsChange).property(&SettingsChange::settings, "settings");
    class_(GameStateUpdate).property(&GameStateUpdate::state, "state");

    class__(PlayedCardsUpdate::ConcealedCardStack, ConcealedCardStack)
        .property(&PlayedCardsUpdate::ConcealedCardStack::cards, "cards")
        .property(&PlayedCardsUpdate::ConcealedCardStack::owner, "owner");
    class_(PlayedCardsUpdate).property(&PlayedCardsUpdate::playedCards, "playedCards");

    class_(Disconnect).property(&Disconnect::reason, "reason");
    class_(GameEnd).property(&GameEnd::history, "history").property(&GameEnd::winner, "winner");
}
FINISH