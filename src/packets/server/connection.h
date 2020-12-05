#pragma once
#include <string>
#include "game.h"
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Responses
{
    struct Welcome
    {
        GameObjects::Connection connection;
    };
    struct Bye
    {
    };
} // namespace Cardsity::Packets::Responses

REGISTER
{
    using namespace Cardsity::Packets::Responses;

    class_(Bye);
    class_(Welcome).property(&Welcome::connection, "connection");
}
FINISH