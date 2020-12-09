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
        std::map<std::uint32_t, GameObjects::WhiteCard> hand;
    };

    struct ChatMessage
    {
        std::string text;
        GameObjects::Player sender;
    };

    struct PlayerLeave
    {
        GameObjects::Player player; // The player who left.
        std::map<con, GameObjects::Player> allPlayers;
    };
    struct PlayerJoin
    {
        GameObjects::Player player; // The player who joined.
        std::map<con, GameObjects::Player> allPlayers;
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
    struct Disconnect
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

    struct GameEnd
    {
        GameObjects::Player winner;
        std::vector<GameObjects::RoundResult> history;
    };
} // namespace Cardsity::Packets::Responses

REGISTER
{
    using namespace Cardsity::Packets::Responses;
    class_(HandUpdate).property(&HandUpdate::hand, "hand").property(&HandUpdate::jokerRequests, "jokerRequests");
    class_(ChatMessage).property(&ChatMessage::sender, "sender").property(&ChatMessage::text, "text");
    class_(PlayerLeave)
        .property(&PlayerLeave::player, "player")
        .property(
            &PlayerLeave::allPlayers, "allPlayers",
            [](const std::map<con, Player> &players) {
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
            [](const std::map<con, Player> &players) {
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