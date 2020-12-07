#pragma once
#include <regex>
#include <string>
#include <cstdint>
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Requests
{
    struct Login
    {
        std::string name;
        std::string color;

        bool valid()
        {
            static const auto nameRegex = std::regex("^.{1,32}$");
            static const auto colorRegex = std::regex("^#[A-F0-9]{6}$");

            if (!std::regex_match(name, nameRegex))
                return false;
            if (!std::regex_match(color, colorRegex))
                return false;

            return true;
        }
    };
    struct Logout
    {
    };

    struct Join
    {
        std::string password;
        std::uint32_t lobbyId;
    };
    struct Leave
    {
    };

    struct ListGames
    {
        std::uint16_t size;
        std::uint16_t offset;
    };
} // namespace Cardsity::Packets::Requests

REGISTER
{
    using namespace Packets::Requests;

    class_(Logout).constructable();
    class_(Login).property(&Login::name, "name").property(&Login::color, "color").constructable();

    class_(Leave).constructable();
    class_(Join).property(&Join::password, "password").constructable();

    class_(ListGames).property(&ListGames::size, "size").property(&ListGames::offset, "offset").constructable();
}
FINISH