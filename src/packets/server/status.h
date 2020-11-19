#pragma once
#include <string>
#include "../../utils/reflection.h"

namespace Cardsity::Packets::Responses
{
    struct GenericStatus
    {
        bool success;
        std::string info;
    };
} // namespace Cardsity::Packets::Responses

REGISTER
{
    using namespace Cardsity::Packets::Responses;
    class_(GenericStatus).property(&GenericStatus::success, "success").property(&GenericStatus::info, "info");
}
FINISH