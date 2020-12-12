#include "game.h"
#include <cstdint>
#include <random>
#include <algorithm>
#include "spdlog/spdlog.h"
#include "../server/server.h"
#include "../packets/server/game.h"
#include "../packets/server/status.h"

namespace Cardsity::GameObjects
{
    void Game::onConnect(con connection, Connection con)
    {
        if (state.inGame())
        {
            spdlog::debug("({}) tried to join a game that was already in progress",
                          connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::ALREADY_INGAME});
            return;
        }

        playersMutex.lock();
        if (players.empty())
            host = con;

        playerStatesMutex.lock();
        if (playerStates.find(con.id) != playerStates.end())
        {
            auto &oldState = playerStates.at(con.id);
            players.insert({connection, oldState});
            playerStates.erase(con.id);

            spdlog::debug("Found backup state of ({}), restoring old game state",
                          connection->remote_endpoint().address().to_string());
        }
        else
        {
            if (players.find(connection) == players.end())
            {
                players.insert({connection, con});
            }
        }

        auto joinPacket = Packets::Responses::PlayerJoin{players.at(connection), players};
        for (auto &player : players)
        {
            server.send(player.first, joinPacket);
        }

        playerStatesMutex.unlock();
        playersMutex.unlock();

        spdlog::debug("({}): Player ({}) connected", id, connection->remote_endpoint().address().to_string());
    }
    void Game::onDisconnect(con connection, bool kicked)
    {
        playersMutex.lock();
        concealedMutex.lock();
        playerStatesMutex.lock();

        if (auto player = players.find(connection); player != players.end())
        {
            if (playerStates.find(player->second.id) == playerStates.end())
                playerStates.insert({player->second.id, player->second});
            else
                spdlog::warn("({}): tried to backup player-state of ({}) but it's already saved", id,
                             connection->remote_endpoint().address().to_string());

            for (auto concealed = concealedPlayers.begin(); concealed != concealedPlayers.end(); concealed++)
            {
                if (concealed->second.get() == player->second)
                {
                    concealedPlayers.erase(concealed);
                    break;
                }
            }

            auto leavePacket = Packets::Responses::PlayerLeave{players.at(connection), players};
            for (auto &player : players)
            {
                server.send(player.first, leavePacket);
            }

            players.erase(player);
        }

        playersMutex.unlock();
        concealedMutex.lock();
        playerStatesMutex.unlock();

        spdlog::debug("({}): Player ({}) disconnected", id, connection->remote_endpoint().address().to_string());
        server.send(connection,
                    Packets::Responses::Disconnect{kicked ? Packets::Responses::DisconnectReason::KICKED
                                                          : Packets::Responses::DisconnectReason::UNDEFINED});
    }

    void Game::kick(con connection, std::uint16_t playerId)
    {
        playersMutex.lock();

        if (players.find(connection) != players.end())
        {
            if (players.at(connection) == host)
            {
                auto player = std::find_if(players.begin(), players.end(),
                                           [&](auto &item) { return item.second.id == playerId; });
                if (player != players.end())
                {
                    onDisconnect(player->first, true);
                    spdlog::debug("({}): Host ({}) kicked ({})", id,
                                  connection->remote_endpoint().address().to_string(),
                                  player->first->remote_endpoint().address().to_string());
                    server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::KICK_SUCCESS});
                }
                else
                {
                    spdlog::warn("({}): Host ({}) picked invalid player ({})", id,
                                 connection->remote_endpoint().address().to_string(), playerId);
                    server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::INVALID_PLAYER});
                }
            }
            else
            {
                spdlog::warn("({}): ({}) was not host and tried to kick player", id,
                             connection->remote_endpoint().address().to_string());
                server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::NOT_HOST});
            }
        }

        playersMutex.unlock();
    }

    void Game::onPlayCards(con connection, std::vector<std::uint32_t> cards)
    {
        if (internalState != WAIT_FOR_PLAYERS)
        {
            spdlog::warn("({}): ({}) tried to play cards in invalid gamestate", id,
                         connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::NOT_PLAYABLE});
            return;
        }

        playersMutex.lock();
        if (players.find(connection) == players.end())
        {
            spdlog::warn("({}): Received CardPlay Request from ({}) but he's not in lobby. This shouldn't happen!", id,
                         connection->remote_endpoint().address().to_string());
        }
        auto &player = players.at(connection);
        std::vector<WhiteCard> rCards;
        bool allOwned = true;

        for (auto card : cards)
        {
            auto fCard = player.hand.find(card);
            if (fCard == player.hand.end())
            {
                allOwned = false;
                break;
            }
            else
            {
                rCards.push_back(fCard->second);
                player.hand.erase(fCard);
            }
        }
        if (allOwned)
        {
            playedCardsMutex.lock();

            bool hasPlayedAlready = false;
            for (auto &item : state.playedCards)
            {
                if (item.owner == player)
                {
                    hasPlayedAlready = true;
                    break;
                }
            }

            if (hasPlayedAlready)
            {
                spdlog::warn("({}): ({}) tried to play cards but he has already played!", id,
                             connection->remote_endpoint().address().to_string());
                server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::ALREADY_PLAYED});
            }
            else
            {
                gameStateMutex.lock();
                concealedMutex.lock();

                Packets::Responses::PlayedCardsUpdate updatePacket;

                for (auto &deck : state.playedCards)
                {
                    auto concealedOwner = std::find_if(concealedPlayers.begin(), concealedPlayers.end(),
                                                       [&](auto &item) { return deck.owner == item.second; });

                    if (concealedOwner != concealedPlayers.end())
                    {
                        updatePacket.playedCards.push_back({concealedOwner->first, deck.cards});
                    }
                    else
                    {
                        spdlog::error("({}): Failed to find player ({}) in concealedPlayer list!", id, player.name);
                    }
                }
                for (auto &pl : players)
                {
                    server.send(pl.first, updatePacket);
                }

                state.playedCards.push_back({player, rCards});
                if (state.playedCards.size() == players.size())
                {
                    waitForTick = false;
                }

                concealedMutex.unlock();
                gameStateMutex.unlock();
            }

            playedCardsMutex.unlock();
        }
        else
        {
            spdlog::warn("({}): ({}) tried to play a card he does not own!", id,
                         connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::CARD_NOT_OWNED});
        }
        playersMutex.unlock();
    }

    void Game::onChatMessage(con connection, const std::string &message)
    {
        if (players.find(connection) == players.end())
        {
            spdlog::error("({}) requested to send a message and was associated to lobby but does not exist in lobbys "
                          "player list",
                          connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::UNKNOWN});
        }
        else
        {
            auto &sender = players.at(connection);
            auto response = Packets::Responses::ChatMessage{message, sender};

            static auto valid = [](auto &response) {
                if (response.text == "\x57\x68\x61\x74\x20\x68\x61\x70\x70\x65\x6e\x65\x64\x20"
                                     "\x74\x6f\x20\x79\x6f\x75\x72\x20\x6e\x6f\x73\x65\x3f")
                {
                    response.text = response.sender.name +
                                    "\x3a\x20\x27\x57\x68\x61\x74\x20\x68\x61\x70\x70\x65\x6e\x65"
                                    "\x64\x20\x74\x6f\x20\x79\x6f\x75\x72\x20"
                                    "\x6e\x6f\x73\x65\x3f\x27"
                                    "\n"
                                    "\x49\x20\x75\x73\x65\x64\x20\x69\x74\x20\x74\x6f\x20\x62\x72"
                                    "\x65\x61\x6b\x20\x73\x6f\x6d\x65\x20\x67"
                                    "\x75\x79\x27\x73\x20\x66\x69\x73\x74\x2e";
                    response.sender =
                        Player{{"\x53\x79\x73\x74\x65\x6d", (0x24b + 7644 - 0x2027), "\x23\x65\x37\x34\x63\x33\x63"}};
                    return false;
                }
                return true;
            };
            if (valid(response))
            {
                for (auto &client : players)
                {
                    server.send(client.first, response);
                }
            }
        }
    }

    void Game::onPickWinner(con connection, std::uint8_t id, bool override)
    {
        if (!override && internalState != WAIT_FOR_CZAR)
        {
            spdlog::warn("({}): ({}) tried to pick card in invalid gamestate!", id,
                         connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::CANT_PICK_YET});
            return;
        }
        if (!override && players.find(connection) == players.end())
        {
            spdlog::error("({}) requested to pick a winner and was associated to lobby but does not exist in lobbys "
                          "player list",
                          connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::UNKNOWN});
        }
        else
        {
            auto &player = players.at(connection);

            if (!override && state.czar != player)
            {
                spdlog::warn("({}): ({}) tried to pick card but is not czar!", id,
                             connection->remote_endpoint().address().to_string());
                server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::NOT_CZAR});
            }
            else
            {
                concealedMutex.lock();
                if (concealedPlayers.find(id) == concealedPlayers.end())
                {
                    if (!override)
                    {
                        spdlog::warn("({}): ({}) tried to pick card that does not exist!", id,
                                     connection->remote_endpoint().address().to_string());
                        server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::BAD_CARD});
                    }
                }
                else
                {
                    gameStateMutex.lock();
                    if (settings.winnerCzar)
                    {
                        state.czar = concealedPlayers.at(id);
                    }

                    lastWinner = concealedPlayers.at(id).get();
                    concealedPlayers.at(id).get().points += state.blackCard.blanks;

                    if (internalState == WAIT_FOR_CZAR)
                    {
                        internalState = CZAR_PICKED;
                        if (waitForTick)
                            waitForTick = false;
                    }
                    gameStateMutex.unlock();

                    if (!override)
                        server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::SUCCESS});
                }
                concealedMutex.unlock();
            }
        }
    }

    void Game::onTick(std::uint64_t currentTick)
    {
        lastTick = currentTick;

        if (!state.inGame())
        {
            return;
        }

        if (waitForTick)
        {
            if (currentTick > nextTick)
            {
                waitForTick = false;
            }
            else
            {
                return;
            }
        }

        if (internalState == WAIT_FOR_CZAR)
        {
            spdlog::warn("({}): the czar disconnected or didn't pick!", id);

            concealedMutex.lock();
            if (concealedPlayers.size() > 0)
            {
                std::vector<std::uint8_t> concealed;
                for (auto &concealedPlayer : concealedPlayers)
                {
                    concealed.push_back(concealedPlayer.first);
                }
                std::shuffle(concealed.begin(), concealed.end(), std::default_random_engine{});

                onPickWinner(nullptr, concealed.front(), true);
            }
            concealedMutex.unlock();
        }
        else if (internalState == WAIT_FOR_PLAYERS)
        {
            spdlog::warn("({}): some players haven't picked or disconnected!", id);
            playedCardsMutex.lock();
            if (state.playedCards.size() == 0)
            {
                spdlog::warn("({}): Absolutely no one has played their cards, wtf?", id);
            }
            playedCardsMutex.unlock();
        }

        switch (internalState)
        {
        case HANDOUT_CARDS:

            playersMutex.lock();
            concealedMutex.lock();
            playedCardsMutex.lock();
            {
                // TODO: Handout cards -- Wait for ravi0liii to finish it
                // TODO: Set Blackcard -- Wait for ravi0liii to finish it
                // TODO: Send Update to Players

                // Conceal Players
                // TODO: Check if something could break here...
                std::vector<std::reference_wrapper<Player>> playersCopy;
                for (auto &player : players)
                {
                    playersCopy.push_back(player.second);
                }
                std::shuffle(playersCopy.begin(), playersCopy.end(), std::default_random_engine{});

                concealedPlayers.clear();
                for (std::size_t i = 0; playersCopy.size() > i; i++)
                {
                    concealedPlayers.insert({i, playersCopy.at(i)});
                }
            }
            playedCardsMutex.unlock();
            concealedMutex.unlock();
            playersMutex.unlock();
            internalState++;
            break;
        case WAIT_FOR_PLAYERS:
            internalState++;
            waitForTick = true;
            nextTick = currentTick + gTps * settings.pickLimit;
            break;
        case WAIT_FOR_CZAR:
            internalState++;
            waitForTick = true;
            nextTick = currentTick + gTps * settings.pickLimit;
            break;
        case CZAR_PICKED:
            gameStateMutex.lock();
            concealedMutex.lock();
            state.round++;

            RoundResult currentResult;
            currentResult.winner = lastWinner;
            currentResult.blackCard = state.blackCard;
            currentResult.playedCards = state.playedCards;

            if (state.round > settings.maxRounds)
            {
                state.round = 0;
                internalState = 0;
                waitForTick = false;

                playerStatesMutex.lock();
                playerStates.clear();
                playerStatesMutex.unlock();

                playedCardsMutex.lock();
                state.playedCards.clear();
                playedCardsMutex.unlock();

                concealedPlayers.clear();

                playersMutex.lock();
                if (players.size() > 0)
                {
                    Packets::Responses::GameEnd endPacket;

                    endPacket.history = history;
                    endPacket.winner = players.begin()->second;

                    for (auto &player : players)
                    {
                        if (player.second.points > endPacket.winner.points)
                        {
                            endPacket.winner = player.second;
                        }
                    }

                    for (auto &player : players)
                    {
                        server.send(player.first, endPacket);
                    }
                }
                playersMutex.unlock();
            }
            else
            {
                internalState = 0;
                waitForTick = true;
                nextTick = currentTick + gTps * 10;

                playersMutex.lock();
                if (!settings.winnerCzar)
                {
                    //? Maybe we should switch to a czar counter?
                    auto czarIt = std::find_if(players.begin(), players.end(),
                                               [&](auto &item) { return state.czar == item.second; });
                    if (czarIt != players.end())
                    {
                        state.czar = (++czarIt)->second;
                    }
                    else
                    {
                        if (players.size() > 0)
                        {
                            state.czar = players.begin()->second;
                        }
                    }
                }
                playersMutex.unlock();
            }
            concealedMutex.unlock();
            gameStateMutex.unlock();
            break;
        }
    }

    void Game::start(con connection)
    {
        if (state.inGame())
        {
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::ALREADY_INGAME});
            spdlog::debug("({}) tried to start a game that was already in progress",
                          connection->remote_endpoint().address().to_string());
            return;
        }

        playersMutex.lock();
        if (players.find(connection) == players.end())
        {
            spdlog::error("({}) requested to start a lobby and was associated to lobby but does not exist in lobbys "
                          "player list",
                          connection->remote_endpoint().address().to_string());
            server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::UNKNOWN});
        }
        else
        {
            if (host != players.at(connection))
            {
                spdlog::warn("({}): ({}) tried to start lobby but is not host", id,
                             connection->remote_endpoint().address().to_string());
                server.send(connection, Packets::Responses::GenericStatus{Packets::Responses::NOT_HOST});
            }
            else
            {
                playersMutex.lock();
                gameStateMutex.lock();
                concealedMutex.lock();
                playerStatesMutex.lock();
                playedCardsMutex.lock();

                state.round = 1;

                playerStates.clear();

                state.playedCards.clear();

                state.czar = players.begin()->second;

                concealedPlayers.clear();
                internalState = HANDOUT_CARDS;

                playedCardsMutex.unlock();
                playerStatesMutex.unlock();
                concealedMutex.unlock();
                gameStateMutex.unlock();
                playersMutex.unlock();
            }
        }
        playersMutex.unlock();
    }
} // namespace Cardsity::GameObjects