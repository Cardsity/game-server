#pragma once
#include <string>
#include <vector>
#include "Json.h"
#include "Connection.h"

using uint = unsigned int;
using namespace nlohmann;

struct Status
{
	std::string message;
	bool success;
};

//! -!-!-!-!-!-!-!-!-!-!-!-
//! -  Pre-Lobby Packets  -
//! -!-!-!-!-!-!-!-!-!-!-!-

struct LoginRequest
{
	std::string name;
	std::string color;

	static bool IsValid(json j)
	{
		if (j["name"].is_string() && j["color"].is_string())
			return true;
		return false;
	}
};

struct UpdateGameRequest
{
	std::string password;

	uint maxJokerRequests;
	float pickLimit;
	uint maxPlayers;
	uint maxRounds;
	uint maxPoints;

	std::vector<std::string> decks;

	static bool IsValid(json j)
	{
		if (j["password"].is_string() &&
			j["pickLimit"].is_number() && j["maxPlayers"].is_number() &&
			j["maxRounds"].is_number() && j["maxPoints"].is_number() &&
			j["decks"].is_array() && j["maxJokerRequests"].is_number() &&
			std::all_of(j["decks"].begin(), j["decks"].end(), [](const json& el) { return el.is_string(); }))
			return true;
		return false;
	}
};

struct CreateGameRequest
{
	std::string name;
	std::string password;

	uint maxJokerRequests;
	float pickLimit;
	uint maxPlayers;
	uint maxRounds;
	uint maxPoints;

	std::vector<std::string> decks;

	static bool IsValid(json j)
	{
		if (j["name"].is_string() && j["password"].is_string() &&
			j["pickLimit"].is_number() && j["maxPlayers"].is_number() &&
			j["maxRounds"].is_number() && j["maxPoints"].is_number() &&
			j["decks"].is_array() && j["maxJokerRequests"].is_number() &&
			std::all_of(j["decks"].begin(), j["decks"].end(), [](const json& el) { return el.is_string(); }))
			return true;
		return false;
	}
};

struct JoinGameRequest
{
	uint lobbyId;
	std::string password;

	static bool IsValid(json j)
	{
		if (j["lobbyId"].is_number() && j["password"].is_string())
			return true;
		return false;
	}
};

struct LobbyListRequest
{
	uint pageNumber;
	uint pageSize;

	static bool IsValid(json j)
	{
		if (j["pageNumber"].is_number() && j["pageSize"].is_number())
			return true;
		return false;
	}
};

//! -!-!-!-!-!-!-!-!-!-!-
//! - In-Lobby Packets  -
//! -!-!-!-!-!-!-!-!-!-!-

struct KickRequest
{
	uint playerId;
	std::string message;

	static bool IsValid(json j)
	{
		if (j["playerId"].is_number() && j["message"].is_string())
			return true;
		return false;
	}
};

//! -!-!-!-!-!-!-!-!-!-
//! - In-Game Packets -
//! -!-!-!-!-!-!-!-!-!-

struct CardPlayRequest
{
	std::vector<std::string> cards;
	static bool IsValid(json j)
	{
		if (j["cards"].is_array() &&
			std::all_of(j["cards"].begin(), j["cards"].end(), [](const json& el) { return el.is_string(); }))
			return true;
		return false;
	}
};

struct JokerCardRequest
{
	std::string text;
	static bool IsValid(json j)
	{
		if (j["text"].is_string())
			return true;
		return false;
	}
};

struct CzarPickRequest
{
	uint winnerId;
	static bool IsValid(json j)
	{
		if (j["winnerId"].is_number())
			return true;
		return false;
	}
};

struct SendChatMessageRequest
{
	std::string message;
	static bool IsValid(json j)
	{
		if (j["message"].is_string())
			return true;
		return false;
	}
};

struct LogoutRequest
{
	bool confirmLogout;
	static bool IsValid(json j)
	{
		if (j["confirmLeave"].is_boolean())
			return true;
		return false;
	}
};

struct LeaveLobbyRequest
{
	bool confirmLeave;
	static bool IsValid(json j)
	{
		if (j["confirmLeave"].is_boolean())
			return true;
		return false;
	}
};

struct StartGameRequest
{
	bool startGame; 
	static bool IsValid(json j)
	{
		if (j["startGame"].is_boolean())
			return true;
		return false;
	}
};