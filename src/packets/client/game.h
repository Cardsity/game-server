#pragma once
#include <regex>
#include <string>
#include <cstdint>
#include "../../objects/game.h"
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Requests
{
    struct CreateGame
    {
        std::string lobbyName;
        GameObjects::GameSettings settings;

        bool valid()
        {
            static auto lobbyNameRegex = std::regex("^.{3,32}$");

            if (!std::regex_match(lobbyName, lobbyNameRegex))
                return false;
            if (!settings.valid())
                return false;

            return true;
        }
    };
    struct ModifyGame
    {
        GameObjects::GameSettings settings;

        bool valid()
        {
            return settings.valid();
        }
    };

    struct PlayCards
    {
        /*Contains card ids*/
        std::vector<std::uint32_t> cards;
    };
    struct JokerCard
    {
        std::string text;

        bool valid()
        {
            static auto contentRegex = std::regex("^.{1,100}$");
            if (!std::regex_match(text, contentRegex))
                return false;

            return true;
        }
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

        bool valid()
        {
            static auto messageRegex = std::regex("^.{1,128}$");
            if (!std::regex_match(text, messageRegex))
                return false;

            return true;
        }
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