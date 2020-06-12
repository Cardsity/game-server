#pragma once
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include "Validation.h"
#include "Responses.h"
#include "serialize.h"
#include "safe_ptr.h"
#include "Requests.h"
#include "Utils.h"
#include "Game.h"
#include "Logs.h"
#include "Json.h"

using wserver = websocketpp::server<websocketpp::config::asio>;
using message_ptr = wserver::message_ptr;
using hdl = websocketpp::connection_hdl;
using websocketpp::frame::opcode::text;
using namespace nlohmann;
using namespace JsonCast;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class CAHServer
{
public:
	template <typename T>
	void send(hdl handle, T what, std::string requestId = "", uint origin = 0)
	{
		if (connections->find(handle) == connections->end())
			return;
		if (origin > 0)
			if (connections->at(handle).lobbyId != origin)
				return;
		

		json j = what;
		if (requestId != "")
			j["requestId"] = requestId;
		if (j.find("success") == j.end())
			j["success"] = true;

		ldebug("Sending Packet: ", j.dump(), " (Origin: ", origin, ")");
		server.send(handle, j.dump(), text);
	}
	void run()
	{
		linfo("Starting server");
		server.listen(9012);
		server.start_accept();
		server.run();
	}
	CAHServer()
	{
		server.clear_access_channels(websocketpp::log::alevel::all);
		server.init_asio();

		server.set_open_handler(bind(&CAHServer::onConnect, this, _1));
		server.set_close_handler(bind(&CAHServer::onDisconnect, this, _1));
		server.set_message_handler(bind(&CAHServer::onMessage, this, _1, _2));

		server.set_pong_timeout(0);
		server.set_close_handshake_timeout(0);
		server.set_open_handshake_timeout(0);

		server.set_reuse_addr(true);
	}
protected:
	sf::safe_ptr<std::map<hdl, Connection, std::owner_less<hdl>>> connections;
	uint idCounter = 0;
	wserver server;

	virtual void onConnect(hdl handle)
	{
		connections->insert({ handle, {++idCounter, handle} });
	}
	virtual void onDisconnect(hdl handle)
	{
		auto con = connections->at(handle);
		if (con.lobbyId > 0)
		{
			if (lobbies->find(con.lobbyId) != lobbies->end())
			{
				lobbies->at(con.lobbyId).disconnect(con);
			}
		}
		connections->erase(handle);
	}
	virtual void onMessage(hdl handle, message_ptr message)
	{
		const auto payload = message->get_payload();
		auto unsafeJson = UnsafeJson::getUnsafeJson(payload);
		if (!unsafeJson.valid)
		{
			server.close(handle, websocketpp::close::status::invalid_payload, "Illegal payload");
			return;
		}
		handlePacket(handle, unsafeJson.json);
	};
#define Is(what) auto rpacket = GetUnsafe<what>(json); rpacket.isValid()
	void handlePacket(hdl handle, json json)
	{
		auto& con = connections->at(handle);
		if (!json["requestId"].is_string())
		{
			send(handle, Status{ "Invalid Packet", false });
			return;
		}
		auto requestId = json["requestId"].get<std::string>();
		ldebug("Received Packet: ", json.dump());

		if (!con.loggedIn)
		{
			if (Is(LoginRequest))
			{
				auto packet = rpacket.get();
				if (con.loggedIn)
				{
					send(handle, Status{ "Already logged in", false }, requestId);
					return;
				}
				if (!stringValidation(packet.name))
				{
					send(handle, Status{ "Invalid name", false }, requestId);
					return;
				}
				if (!colorValidation(packet.color))
				{
					send(handle, Status{ "Invalid color", false }, requestId);
					return;
				}
				if (sfMContains<Connection>(connections, packet.name))
				{
					send(handle, Status{ "Duplicate name", false }, requestId);
					return;
				}
				con.loggedIn = true;
				con.name = packet.name;
				con.color = packet.color;
				send(handle, Welcome{ con }, requestId);
				return;
			}
		}

		if (con.loggedIn)
		{
			if (Is(LogoutRequest))
			{
				auto packet = rpacket.get();
				if (packet.confirmLogout)
				{
					if (con.lobbyId > 0)
					{
						if (lobbies->find(con.lobbyId) != lobbies->end())
						{
							lobbies->at(con.lobbyId).disconnect(con);
						}
					}
					con.loggedIn = false;
					return;
				}
				return;
			}
			if (Is(LobbyListRequest))
			{
				auto packet = rpacket.get();
				if (packet.pageNumber < 0 || packet.pageSize < 0)
				{
					send(handle, Status{ "Invalid arguments", false }, requestId);
					return;
				}

				auto start = (packet.pageNumber - 1) * packet.pageSize;
				auto end = (packet.pageNumber * packet.pageSize);

				std::vector<LobbyInfo> selectedLobbies;
				for (auto l : sfslice<Lobby, LobbyInfo>(lobbies, start, end))
				{
					if (l.isValid)
						selectedLobbies.push_back(l);
				}

				send(handle, LobbyListResponse{ selectedLobbies, lobbies->size() }, requestId);
				return;
			}
			if (Is(CreateGameRequest))
			{
				auto packet = rpacket.get();
				if (con.lobbyId > 0)
				{
					send(handle, Status{ "You're already in a lobby!", false }, requestId);
					return;
				}
				if (packet.decks.size() < 1 || packet.decks.size() > 10)
				{
					send(handle, Status{ "Too few decks!", false }, requestId);
					return;
				}
				if (
					packet.maxPlayers > 15 || packet.maxPlayers < 3
					|| packet.maxPoints > 25 || packet.maxPoints < 5
					|| packet.maxRounds > 20 || packet.maxRounds < 5
					|| packet.pickLimit > 5 || packet.pickLimit < 0.5
					|| packet.maxJokerRequests < 0 || packet.maxJokerRequests > 10
					)
				{
					send(handle, Status{ "Invalid game settings!", false }, requestId);
					return;
				}
				if (!stringValidation(packet.name))
				{
					send(handle, Status{ "Invalid name!", false }, requestId);
					return;
				}
				if (!passwordValidation(packet.password))
				{
					send(handle, Status{ "Bad password!", false }, requestId);
					return;
				}


				Lobby lobby;
				for (auto deck : packet.decks)
				{
					lobby.addDeck(deck);
				}
				lobby.id = ++idCounter;
				lobby.name = packet.name;
				lobby.password = packet.password;
				lobby.pickLimit = packet.pickLimit * 60'000;
				lobby.maxRounds = packet.maxRounds;
				lobby.maxPlayers = packet.maxPlayers;
				lobby.maxPoints = packet.maxPoints;
				lobby.maxJokerRequests = packet.maxJokerRequests;

				lobbies->insert({ lobby.id, lobby });
				lobbies->at(lobby.id).forceConnect(con);
				con.lobbyId = lobby.id;

				send(handle, LobbyStatus(lobbies->at(lobby.id)), requestId);
				return;
			}
			if (Is(JoinGameRequest))
			{
				auto packet = rpacket.get();
				if (lobbies->find(packet.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				auto response = lobbies->at(packet.lobbyId).connect(con, packet);
				if (response.success)
				{
					con.lobbyId = packet.lobbyId;
					send(handle, LobbyStatus(lobbies->at(packet.lobbyId)), requestId);
				}
				else
					send(handle, response, requestId);
				return;
			}
			if (Is(StartGameRequest))
			{
				auto packet = rpacket.get();
				if (!packet.startGame)
					return;
				if (con.lobbyId <= 0)
				{
					send(handle, Status{ "You're not in a lobby", false }, requestId);
					return;
				}
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				send(handle, lobbies->at(con.lobbyId).startGame(con), requestId);
				return;
			}
			if (Is(KickRequest))
			{
				auto packet = rpacket.get();
				if (con.lobbyId <= 0)
				{
					send(handle, Status{ "You're not in a lobby", false }, requestId);
					return;
				}
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				auto res = lobbies->at(con.lobbyId).kickPlayer(con, packet);
				if (res.success)
				{
					foreach(pcon, connections)
					{
						if (pcon->second.id == packet.playerId)
						{
							pcon->second.lobbyId = 0;
							send(pcon->second.hdl, KickNotify{ packet.message });
							break;
						}
					}
				}
				send(handle, res, requestId);
				return;
			}
			if (Is(CardPlayRequest))
			{
				auto packet = rpacket.get();
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				send(handle, lobbies->at(con.lobbyId).playCards(con, packet), requestId);
				return;
			}
			if (Is(JokerCardRequest))
			{
				auto packet = rpacket.get();
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				send(handle, lobbies->at(con.lobbyId).jokerRequest(con, packet), requestId);
				return;
			}
			if (Is(CzarPickRequest))
			{
				auto packet = rpacket.get();
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				send(handle, lobbies->at(con.lobbyId).czarPick(con, packet), requestId);
				return;
			}
			if (Is(SendChatMessageRequest))
			{
				auto packet = rpacket.get();
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				send(handle, lobbies->at(con.lobbyId).sendChatMessage(con, packet), requestId);
				return;
			}
			if (Is(LeaveLobbyRequest))
			{
				auto packet = rpacket.get();
				if (lobbies->find(con.lobbyId) == lobbies->end())
				{
					send(handle, Status{ "Lobby not found!", false }, requestId);
					return;
				}
				auto res = lobbies->at(con.lobbyId).disconnect(con);
				if (res.success)
				{
					con.lobbyId = 0;
				}
				send(handle, res, requestId);
				return;
			}
		}
	}
#undef Is
};

inline CAHServer server;