#pragma once
#include <string>
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Responses
{
    enum Status : std::uint8_t
    {
        BAD_NAME_OR_COLOR,
        INVALID_REQUEST,
        INVALID_PLAYER,
        INVALID_GAME,
        BAD_MESSAGE,
        BAD_PASSWORD,
        NOT_CZAR,
        NOT_HOST,
        CARD_NOT_OWNED,
        UNKNOWN,
        ALREADY_PLAYED,
        KICK_SUCCESS,
        ALREADY_INGAME,
        CARD_ADDED,
        SUCCESS,
        EXPECTED_AUTH,
        BAD_GAME_SETTINGS,
    };
    struct GenericStatus
    {
        Status status;
    };
} // namespace Cardsity::Packets::Responses

REGISTER
{
    using namespace Cardsity::Packets::Responses;
    class_(GenericStatus).property(&GenericStatus::status, "status");
}
FINISH