#pragma once
#include <cstdint>
#include <string>
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Requests
{
    struct Login
    {
        std::string name;
        std::string color;
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

    class_(Logout);
    class_(Login).property(&Login::name, "name").property(&Login::color, "color");

    class_(Leave);
    class_(Join).property(&Join::password, "password");

    class_(ListGames).property(&ListGames::size, "size").property(&ListGames::offset, "offset");
}
FINISH