#pragma once
#include <string>
#include "../../objects/game.h"
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Responses
{
    struct HandUpdate
    {
        GameObjects::Hand hand;
    };

    struct ChatMessage
    {
        std::string text;
        GameObjects::Player sender;
    };
} // namespace Cardsity::Packets::Responses

REGISTER
{
    using namespace Cardsity::Packets::Responses;
}
FINISH