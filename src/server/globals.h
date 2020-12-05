#pragma once
#include <cstdint>
#include <server_ws.hpp>

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using con = std::shared_ptr<WsServer::Connection>;
using msg = std::shared_ptr<WsServer::InMessage>;
inline std::uint16_t gTps = 50;