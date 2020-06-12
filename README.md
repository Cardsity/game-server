# Cardsity Game Server

[![Last Commit](https://img.shields.io/github/last-commit/Cardsity/game-server?style=for-the-badge)](https://github.com/Cardsity/game-server/commits)
[![Docker Image](https://img.shields.io/github/workflow/status/Cardsity/game-server/Docker%20Image%20CI?label=docker%20image&style=for-the-badge)](https://github.com/Cardsity/game-server/actions?query=workflow%3A%22Docker+Image+CI%22)
[![License](https://img.shields.io/github/license/Cardsity/game-server?style=for-the-badge)](https://github.com/Cardsity/game-server/blob/master/LICENSE)

# Running

## Docker

Build docker image

```
docker build -t cardsity-game-server .
```

Run docker image

```
docker run -it -p 9012:9012 --rm --name cardsity-game-server cardsity-game-server
```

## Compile it yourself

To compile this yourself you will need to link [websocketpp](https://github.com/zaphoyd/websocketpp) and [cpr](https://github.com/whoshuu/cpr).
It&#39;s recommended to install these through [vcpkg](https://github.com/Microsoft/vcpkg).

*Note: To get everything running you have to change the card-deck server endpoint in the "addDeck" function found in the Lobby.cpp unless you are running the deck-server on the same network*

# Making your own client

<details>
  <summary>Content</summary>

## Connection and basic packet structure

This is a websocket server that runs on port 9012.

Packets are always sent in json format.

For every packet you send you are expected to specify a requestId - which should always be unique.  
The vue-client uses the library [websocket-as-promised](https://github.com/vitalets/websocket-as-promised) which implements requestId's by default.

All packets you receive will have the requestId you specified in them.

## Flow
The first packet you are expected to send is a [LoginRequest](#LoginRequest).  
Then you can send a [LobbyListRequest](#LobbyListRequest).  
Then a [CreateGameRequest](#CreateGameRequest) or a [JoinGameRequest](#JoinGameRequest).  

List of packets that can be sent while in a lobby:
* Regardless of Lobby-State 
	* [KickRequest](#KickRequest) * Only works as host
	* [LeaveLobbyRequest](#LeaveLobbyRequest)
	* [SendChatMessageRequest](#SendChatMessageRequest)
* While in lobby state
	* [StartGameRequest](#StartGameRequest) * Only works as host
* While in game
	* [CardPlayRequest](#CardPlayRequest) * Only works as player
	* [JokerCardRequest](#JokerCardRequest)
	* [CzarPickRequest](#CzarPickRequest) * Only works as czar

## Packets
### Custom Types - Sent in Response
#### Connection
```
int: id
int: lobbyId
string: name
string: color
```
#### WhiteCard
```
string: text
int: ownerId
```
#### BlackCard
```
string: text
int: blanks
```
#### Player
```
Connection: owner
int: points
```
#### DeckInfo
```
string: name
```
#### LobbyStatus
```
arrayOf(Player): players
arrayOf(DeckInfo): decks
BlackCard: blackCard
int: currentRound
string: name
Player: czar
int: id

float: pickLimit
int: maxPlayers
int: maxRounds
int: maxPoints
```
#### LobbyInfo
```
int: id
bool: password
int: maxPlayers
string: name
int: playerCount
string: hostName
```
### Requests
| Packet 	| Content 	| Allowed in Lobby 	| Allowed In-Game 	| Allowed as Player 	| Allowed as Czar 	| Only as Host 	|
|-	|-	|-	|-	|-	|-	|-	|
| LoginRequest 	| string: name (^[a-zA-Z_\-0-9äüö ]{3,16}$)<br>string: color [in Hex] (^#[A-F0-9]{6}$) 	| :x: 	| :x: 	| :x: 	| :x: 	| :x: 	|
| CreateGameRequest 	| string: name (^[a-zA-Z_\-0-9äüö ]{3,16}$)<br>string: password [empty string if none] (^[a-zA-Z_\-0-9äüö]{0,16}$)<br>float: pickLimit (min: 0.5, max: 5)<br>int: maxPlayers (min: 3, max: 15)<br>int: maxRounds (min: 5, max 20)<br>int: maxPoints (min: 5, max: 25)<br>arrayOf(string): decks (contains the deckIds as string) 	| :x: 	| :x: 	| :x: 	| :x: 	| :x: 	|
| JoinGameRequest 	| int: lobbyId<br>string: password [empty string if none] (^[a-zA-Z_\-0-9äüö]{0,16}$) 	| :x: 	| :x: 	| :x: 	| :x: 	| :x: 	|
| LobbyListRequest 	| int: pageNumber<br>int: pageSize 	| :x: 	| :x: 	| :x: 	| :x: 	| :x: 	|
| KickRequest 	| int: playerId<br>string: message (^.{1,64}$) 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	| :x: 	| :heavy_check_mark: 	|
| CardPlayRequest 	| arrayOf(string): cards (Text of the cards you want to play - you have to own them though) 	| :x: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	| :x: 	|
| JokerCardRequest 	| string: text (^.{1,64}$) 	| :x: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	|
| CzarPickRequest 	| int: winnerId - Id of the selected card owner 	| :x: 	| :heavy_check_mark: 	| :x: 	| :heavy_check_mark: 	| :x: 	|
| SendChatMessageRequest 	| string: message (^.{1,64}$) 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	|
| LogoutRequest 	| bool: confirmLogout 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	|
| LeaveLobbyRequest 	| bool: confirmLeave 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	|
| StartGameRequest 	| bool: startGame 	| :heavy_check_mark: 	| :x: 	| :x: 	| :x: 	| :heavy_check_mark: 	|
### Responses
| Packet 	| Content 	| Sent In Lobby 	| Sent In-Game 	| Contains requestId 	| Answer to 	|
|-	|-	|-	|-	|-	|-	|
| Status 	| string: message<br>bool: success 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :heavy_check_mark: 	| On Error: Everything<br>On Success: KickRequest, CardPlayRequest, JokerCardRequest, CzarPickRequest, SendChatMessageRequest, LeaveLobbyRequest, StartGameRequest 	|
| Welcome 	| Connection: you 	| :x: 	| :x: 	| :heavy_check_mark: 	| Login Request 	|
| UnrevealedCard 	| int: unrevealedCardOwnerId 	| :x: 	| :heavy_check_mark: 	| :x: 	| None 	|
| RevealCards 	| arrayOf(WhiteCard): cards 	| :x: 	| :heavy_check_mark: 	| :x: 	| None 	|
| CzarPickNotify 	| int: winnerId 	| :x: 	| :heavy_check_mark: 	| :x: 	| None 	|
| SentChatMessage 	| Connection: sender<br>string: message 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	| None 	|
| HandUpdate 	| arrayOf(WhiteCard): newHand 	| :x: 	| :heavy_check_mark: 	| :x: 	| None 	|
| GameEnd 	| Player: winner<br>arrayOf(tuple(Connection, BlackCard, arrayOf(WhiteCard))) cardHistory 	| :x: 	| :heavy_check_mark: 	| :x: 	| None 	|
| KickNotify 	| string: kickReason 	| :heavy_check_mark: 	| :heavy_check_mark: 	| :x: 	| None 	|
| LobbyListResponse 	| arrayOf(LobbyInfo): lobbies<br>int: totalRows 	| :x: 	| :x: 	| :heavy_check_mark: 	| LobbyListRequest 	|

</details>