#pragma once
#include <string>
#include <cstdint>
#include <server_ws.hpp>

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

using rawcon = std::shared_ptr<WsServer::Connection>;
struct con
{
    rawcon raw;
    std::string requestId;
    std::string remote_endpoint() const
    {
        return raw->remote_endpoint().address().to_string();
    }
    con()
    {
    }
    con(const rawcon &o) : raw(o), requestId("")
    {
    }
    con(rawcon con, const std::string &requestId) : raw(con), requestId(requestId)
    {
    }
    operator rawcon()
    {
        return raw;
    }
};

using msg = std::shared_ptr<WsServer::InMessage>;
inline std::uint16_t gTps = 50;