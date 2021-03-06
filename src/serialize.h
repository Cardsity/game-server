#pragma once
#include "Responses.h"
#include "Requests.h"
#include "Json.h"
using namespace nlohmann;

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const Connection& p) {
	j = json{ {"id", p.id},{"lobbyId", p.lobbyId},{"name", p.name},{"color", p.color},{"name", p.name} };
}
inline void from_json(const json& j, Connection& p) {
	j.at("id").get_to(p.id);
	j.at("lobbyId").get_to(p.lobbyId);
	j.at("name").get_to(p.name);
	j.at("color").get_to(p.color);
	j.at("name").get_to(p.name);
}
/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const WhiteCard& p) {
	j = json{ {"text", p.text}, {"ownerId", p.ownerId} };
}
inline void to_json(json& j, const BlackCard& p) {
	j = json{ {"text", p.text},{"blanks", p.blanks} };
}
inline void to_json(json& j, const Deck& p) {
	j = json{ {"whiteCards", p.whiteCards},{"blackCards", p.blackCards},{"name", p.name},{"id", p.id} };
}
inline void to_json(json& j, const Player& p) {
	j = json{ {"owner", p.owner},{"points", p.points} };
}
inline void from_json(const json& j, WhiteCard& p) {
	j.at("text").get_to(p.text);
}
inline void from_json(const json& j, BlackCard& p) {
	j.at("text").get_to(p.text);
	j.at("blanks").get_to(p.blanks);
}
inline void from_json(const json& j, Deck& p) {
	j.at("whiteCards").get_to(p.whiteCards);
	j.at("blackCards").get_to(p.blackCards);
	j.at("name").get_to(p.name);
	j.at("id").get_to(p.id);
}
inline void from_json(const json& j, Player& p) {
	j.at("owner").get_to(p.owner);
	j.at("points").get_to(p.points);
}

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const Welcome& p) {
	j = json{ {"you", p.you} };
}
inline void from_json(const json& j, Welcome& p) {
	j.at("you").get_to(p.you);
}


/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const Status& p) {
	j = json{ {"message", p.message},{"success", p.success} };
}
inline void to_json(json& j, const LoginRequest& p) {
	j = json{ {"name", p.name},{"color", p.color} };
}
inline void to_json(json& j, const JoinGameRequest& p) {
	j = json{ {"lobbyId", p.lobbyId},{"password", p.password} };
}
inline void to_json(json& j, const LobbyListRequest& p) {
	j = json{ {"pageNumber", p.pageNumber},{"pageSize", p.pageSize} };
}
inline void to_json(json& j, const KickRequest& p) {
	j = json{ {"playerId", p.playerId},{"message", p.message} };
}
inline void to_json(json& j, const CardPlayRequest& p) {
	j = json{ {"cards", p.cards} };
}
inline void to_json(json& j, const JokerCardRequest& p) {
	j = json{ {"text", p.text} };
}
inline void to_json(json& j, const CzarPickRequest& p) {
	j = json{ {"winnerId", p.winnerId} };
}
inline void to_json(json& j, const SendChatMessageRequest& p) {
	j = json{ {"message", p.message} };
}
inline void to_json(json& j, const LeaveLobbyRequest& p) {
	j = json{ {"confirmLeave", p.confirmLeave} };
}
inline void from_json(const json& j, Status& p) {
	j.at("message").get_to(p.message);
	j.at("success").get_to(p.success);
}
inline void from_json(const json& j, LoginRequest& p) {
	j.at("name").get_to(p.name);
	j.at("color").get_to(p.color);
}
inline void from_json(const json& j, JoinGameRequest& p) {
	j.at("lobbyId").get_to(p.lobbyId);
	j.at("password").get_to(p.password);
}
inline void from_json(const json& j, LobbyListRequest& p) {
	j.at("pageNumber").get_to(p.pageNumber);
	j.at("pageSize").get_to(p.pageSize);
}
inline void from_json(const json& j, KickRequest& p) {
	j.at("playerId").get_to(p.playerId);
	j.at("message").get_to(p.message);
}
inline void from_json(const json& j, CardPlayRequest& p) {
	j.at("cards").get_to(p.cards);
}
inline void from_json(const json& j, JokerCardRequest& p) {
	j.at("text").get_to(p.text);
}
inline void from_json(const json& j, CzarPickRequest& p) {
	j.at("winnerId").get_to(p.winnerId);
}
inline void from_json(const json& j, SendChatMessageRequest& p) {
	j.at("message").get_to(p.message);
}
inline void from_json(const json& j, LeaveLobbyRequest& p) {
	j.at("confirmLeave").get_to(p.confirmLeave);
}

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const DeckInfo& p) {
	j = json{ {"name", p.name}, {"id", p.id} };
}
inline void from_json(const json& j, DeckInfo& p) {
	j.at("name").get_to(p.name);
	j.at("id").get_to(p.id);
}

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const LobbyInfo& p) {
	j = json{ {"id", p.id},{"password", p.password},{"maxPlayers", p.maxPlayers},{"name", p.name},{"playerCount", p.playerCount},{"hostName", p.hostName} };
}
inline void from_json(const json& j, LobbyInfo& p) {
	j.at("id").get_to(p.id);
	j.at("password").get_to(p.password);
	j.at("maxPlayers").get_to(p.maxPlayers);
	j.at("name").get_to(p.name);
	j.at("playerCount").get_to(p.playerCount);
	j.at("hostName").get_to(p.hostName);
}
/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const UnrevealedCard& p) {
	j = json{ {"unrevealedCardOwnerId", p.unrevealedCardOwnerId} };
}
inline void to_json(json& j, const RevealCards& p) {
	j = json{ {"cards", p.cards} };
}
inline void to_json(json& j, const CzarPickNotify& p) {
	j = json{ {"winnerId", p.winnerId} };
}
inline void to_json(json& j, const SentChatMessage& p) {
	j = json{ {"sender", p.sender},{"message", p.message} };
}
inline void to_json(json& j, const GameEnd& p) {
	j = json{ {"winner", p.winner}, {"cardHistory", p.cardHistory} };
}
inline void to_json(json& j, const KickNotify& p) {
	j = json{ {"kickReason", p.kickReason} };
}
inline void to_json(json& j, const LobbyStatus& p) {
	j = json{ {"players", p.players},{"decks", p.decks},{"password", p.password},{"blackCard", p.blackCard},{"currentRound", p.currentRound},{"name", p.name},{"czar", p.czar},{"id", p.id},{"winnerBecomesCzar", p.winnerBecomesCzar},{"jokerCardsToDeck", p.jokerCardsToDeck},{"maxJokerRequests", p.maxJokerRequests},{"pickLimit", p.pickLimit},{"maxPlayers", p.maxPlayers},{"maxRounds", p.maxRounds},{"maxPoints", p.maxPoints} };
}
inline void from_json(const json& j, LobbyStatus& p) {
	j.at("players").get_to(p.players);
	j.at("decks").get_to(p.decks);
	j.at("password").get_to(p.password);
	j.at("blackCard").get_to(p.blackCard);
	j.at("currentRound").get_to(p.currentRound);
	j.at("name").get_to(p.name);
	j.at("czar").get_to(p.czar);
	j.at("id").get_to(p.id);
	j.at("winnerBecomesCzar").get_to(p.winnerBecomesCzar);
	j.at("jokerCardsToDeck").get_to(p.jokerCardsToDeck);
	j.at("maxJokerRequests").get_to(p.maxJokerRequests);
	j.at("pickLimit").get_to(p.pickLimit);
	j.at("maxPlayers").get_to(p.maxPlayers);
	j.at("maxRounds").get_to(p.maxRounds);
	j.at("maxPoints").get_to(p.maxPoints);
}
inline void to_json(json& j, const LobbyListResponse& p) {
	j = json{ {"lobbies", p.lobbies},{"totalRows", p.totalRows} };
}
inline void from_json(const json& j, UnrevealedCard& p) {
	j.at("unrevealedCardOwnerId").get_to(p.unrevealedCardOwnerId);
}
inline void from_json(const json& j, RevealCards& p) {
	j.at("cards").get_to(p.cards);
}
inline void from_json(const json& j, CzarPickNotify& p) {
	j.at("winnerId").get_to(p.winnerId);
}
inline void from_json(const json& j, SentChatMessage& p) {
	j.at("sender").get_to(p.sender);
	j.at("message").get_to(p.message);
}
inline void from_json(const json& j, GameEnd& p) {
	j.at("winner").get_to(p.winner);
	j.at("cardHistory").get_to(p.cardHistory);
}
inline void from_json(const json& j, KickNotify& p) {
	j.at("kickReason").get_to(p.kickReason);
}
inline void from_json(const json& j, LobbyListResponse& p) {
	j.at("lobbies").get_to(p.lobbies);
	j.at("totalRows").get_to(p.totalRows);
}
/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const StartGameRequest& p) {
	j = json{ {"startGame", p.startGame} };
}
inline void from_json(const json& j, StartGameRequest& p) {
	j.at("startGame").get_to(p.startGame);
}

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const LogoutRequest& p) {
	j = json{ {"confirmLogout", p.confirmLogout} };
}
inline void from_json(const json& j, LogoutRequest& p) {
	j.at("confirmLogout").get_to(p.confirmLogout);
}

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const HandUpdate& p) {
	j = json{ {"newHand", p.newHand},{"isCausedByJokerRequest", p.isCausedByJokerRequest},{"jokerRequestsRemaining", p.jokerRequestsRemaining} };
}
inline void from_json(const json& j, HandUpdate& p) {
	j.at("newHand").get_to(p.newHand);
	j.at("isCausedByJokerRequest").get_to(p.isCausedByJokerRequest);
	j.at("jokerRequestsRemaining").get_to(p.jokerRequestsRemaining);
}

/*Code was auto generated by FSR by Curve*/
inline void to_json(json& j, const UpdateGameRequest& p) {
	j = json{ {"password", p.password},{"winnerBecomesCzar", p.winnerBecomesCzar},{"jokerCardsToDeck", p.jokerCardsToDeck},{"maxJokerRequests", p.maxJokerRequests},{"pickLimit", p.pickLimit},{"maxPlayers", p.maxPlayers},{"maxRounds", p.maxRounds},{"maxPoints", p.maxPoints},{"decks", p.decks} };
}
inline void to_json(json& j, const CreateGameRequest& p) {
	j = json{ {"name", p.name},{"password", p.password},{"winnerBecomesCzar", p.winnerBecomesCzar},{"jokerCardsToDeck", p.jokerCardsToDeck},{"maxJokerRequests", p.maxJokerRequests},{"pickLimit", p.pickLimit},{"maxPlayers", p.maxPlayers},{"maxRounds", p.maxRounds},{"maxPoints", p.maxPoints},{"decks", p.decks} };
}
inline void from_json(const json& j, UpdateGameRequest& p) {
	j.at("password").get_to(p.password);
	j.at("winnerBecomesCzar").get_to(p.winnerBecomesCzar);
	j.at("jokerCardsToDeck").get_to(p.jokerCardsToDeck);
	j.at("maxJokerRequests").get_to(p.maxJokerRequests);
	j.at("pickLimit").get_to(p.pickLimit);
	j.at("maxPlayers").get_to(p.maxPlayers);
	j.at("maxRounds").get_to(p.maxRounds);
	j.at("maxPoints").get_to(p.maxPoints);
	j.at("decks").get_to(p.decks);
}
inline void from_json(const json& j, CreateGameRequest& p) {
	j.at("name").get_to(p.name);
	j.at("password").get_to(p.password);
	j.at("winnerBecomesCzar").get_to(p.winnerBecomesCzar);
	j.at("jokerCardsToDeck").get_to(p.jokerCardsToDeck);
	j.at("maxJokerRequests").get_to(p.maxJokerRequests);
	j.at("pickLimit").get_to(p.pickLimit);
	j.at("maxPlayers").get_to(p.maxPlayers);
	j.at("maxRounds").get_to(p.maxRounds);
	j.at("maxPoints").get_to(p.maxPoints);
	j.at("decks").get_to(p.decks);
}
