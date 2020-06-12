#pragma once
#include <string>
#include <websocketpp/server.hpp>

using uint = unsigned int;
using handle = websocketpp::connection_hdl;

struct Connection
{
	uint id;
	handle hdl; //SERIALIZEPRIV
	uint lobbyId;
	bool loggedIn; //SERIALIZEPRIV
	std::string name;
	std::string color;

	bool operator==(const Connection& o)
	{
		return o.id == this->id;
	}
	bool operator!=(const Connection& o)
	{
		return o.id != this->id;
	}
	bool operator==(const uint& o)
	{
		return this->id == o;
	}
	bool operator==(const std::string& o)
	{
		return name == o;
	}
};