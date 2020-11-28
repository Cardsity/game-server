#pragma once
#include <cstdint>
#include <string>
#include "../../objects/game.h"
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Requests
{
    struct CreateGame
    {
        std::string lobbyName;
        GameObjects::GameSettings settings;
    };
    struct ModifyGame
    {
        GameObjects::GameSettings settings;
    };

    struct PlayCards
    {
        /*Contains card ids*/
        std::vector<std::uint32_t> cards;
    };
    struct JokerCard
    {
        std::string text;
    };
    struct PickCard
    {
        std::uint16_t id;
    };

    struct Kick
    {
        std::uint16_t id;
    };
    struct Start
    {
    };

    struct SendMessage
    {
        std::string text;
    };
} // namespace Cardsity::Packets::Requests

REGISTER
{
    using namespace Packets::Requests;

    class_(CreateGame)
        .property(&CreateGame::lobbyName, "lobbyName")
        .property(&CreateGame::settings, "settings")
        .constructable();
    class_(ModifyGame).property(&ModifyGame::settings, "settings").constructable();

    class_(PlayCards).property(&PlayCards::cards, "cards").constructable();
    class_(JokerCard).property(&JokerCard::text, "text").constructable();
    class_(PickCard).property(&PickCard::id, "id").constructable();

    class_(Kick).property(&Kick::id, "id").constructable();
    class_(Start).constructable();

    class_(SendMessage).property(&SendMessage::text, "text").constructable();
}
FINISH