#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Server.cpp
*
* The server is built using a TCP socket and multithreading allowing multiple games to be played at once. the server
* socket connection is set up where it then listens for incoming connection. The server also stores all of the player 
* information (player ID, username), game information (gameID, game board), which players turn it is, and the outcome 
* of the game. 
*/


#include <cstdio>
#include <iostream>
#include <memory>
#include <sys/types.h>		//socket, bind
#include <sys/socket.h>		//coket, bind, listen, inet_ntoa
#include <netinet/in.h>		//hton1, htons, inet_ntoa
#include <arpa/inet.h>		//intet_ntoa
#include <netdb.h>			//gethostbyname
#include <unistd.h>			//read, write, close 
#include <strings.h>		//bzero
#include <netinet/tcp.h>	//SO_REUSEADDR
#include <sys/uio.h>		//writev
#include <pthread.h>        //pthread
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <fcntl.h>  
#include <map>
#include "Server.h"
#include "CommandType.h"
#include "Client.h"
#include "GameOutcome.h"

//sets up the server side socket and listens for incoming connections. 
void Server::Start(int port) {

    //set up addr atructure 
    struct sockaddr_in hints;
    memset(&hints, 0, sizeof(hints));

    hints.sin_family = AF_INET; // IPv4 or v6
    hints.sin_port = htons(port);

    //create socket 
    this->sd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sd < 0) {
        throw new SocketException("Error creating server socket");
    }

    //lose the "addres already in use" error message
    const int yes = 1;
    setsockopt(this->sd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

    //Bind socket 
    int err = bind(this->sd, (sockaddr*)&hints, sizeof(hints));
    if (err < 0) {
        throw new SocketException("Unable to bind", err);
    }

    //listen for connection requests 
    err = listen(this->sd, 5);
    if (err < 0) {
        throw new SocketException("Unablle to listen", err);
    }

    std::cout << "Server ready and accepting connections\n";

    //keep accepting connections until forceful quit 
    while (1) {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);

        //accept connections
        int clientSd = accept(this->sd, (sockaddr*)&client, &clientSize);
        std::cout << "Connection accepted: Sd=" << clientSd << std::endl;

        // Create a new thread to process requests from this client.
        ClientContext* context = new ClientContext(this, clientSd);
        int err = pthread_create(&context->threadId, NULL, &Server::threadProc, context);
        /*if (err < 0) {
            delete context;
            throw new SocketException("Unable to create new thread for client", err);
        }

        context = NULL;*/
    }
}

//register a user
void Server::registerUser(std::shared_ptr<ClientContext> context) {

    // Get the name of the user.
    std::string name = context->client.readString();

    // Assign a unique id.
    std::shared_ptr<User> user = std::make_shared<User>();
    user->id = this->nextUserId++;
    user->name = name;

    // Add to the map.
    this->users.insert({ user->id, user });

    // Display that user has been registered.
    std::cout << "Register User: Id=" << user->id << ", Name = " << name << std::endl;

    // Save the user id and send it to the user.
    context->userId = user->id;
    context->client.sendInt(user->id);
}

//start a new game
void Server::startNewGame(std::shared_ptr<ClientContext> context) {
    std::shared_ptr<User> user = this->users.at(context->userId);
    std::shared_ptr<Game> game = std::make_shared<Game>(this->nextGameId++, user->id, user->name);
    //insert into waiting games map 
    this->waitingGames.insert({ game->id, game });
    std::cout << "Created Game " << game->id << " for user " << user->id << std::endl;
    //send the game to the client 
    context->client.sendGame(game);
}

//get all the waiting games 
void Server::getWaitingGames(std::shared_ptr<ClientContext> context) {
    //send client the sise of map of waiting games 
    context->client.sendInt(this->waitingGames.size());
    for (auto game : this->waitingGames) {
        //send the game
        context->client.sendGame(game.second);
    }
}

//join a game
void Server::joinGame(std::shared_ptr<ClientContext> context) {
    int id = context->client.readInt();

    // Get the user who will be joining.
    std::shared_ptr<User> user = this->users.at(context->userId);

    // Remove the game from waiting list.
    std::shared_ptr<Game> game = this->waitingGames.at(id);
    this->waitingGames.erase(id);

    // Add it to the active list after updating it.
    game->user2 = user->id;
    game->userName2 = user->name;

    // Now add the game to the active list.
    this->activeGames.insert({ id, game });

    //send the game
    context->client.sendGame(game);
}

//see if the game has started 
void Server::isGameStarted(std::shared_ptr<ClientContext> context) {
    int gameId = context->client.readInt();
    int result = (this->activeGames.find(gameId) != this->activeGames.end());
    context->client.sendInt(result);
}

//check to see whos turn it is 
void Server::isMyTurn(std::shared_ptr<ClientContext> context) {
    //get the game ID
    int gameId = context->client.readInt();
    //see if it is in the active games map 
    auto game = this->activeGames.find(gameId);
    //send whos turn it is - send user ID
    int result = game->second->turnOfUser == context->userId;
    //check to see if someone has won or tie 
    int outcome = checkOutcome(game->second);

    //check to see who won 
    if (outcome == GameOutcome::User1Win || outcome == GameOutcome::User2Win) {
        outcome = context->userId == game->second->winnerId ? GameOutcome::YouWin : GameOutcome::YouLose;
    }

    //send the outcome 
    context->client.sendInt(outcome);
    //send the result 
    context->client.sendInt(result);
    //send the game 
    context->client.sendGame(game->second);
}

//helper method to see if there is a win or tie 
bool Server::didWin(std::shared_ptr<Game> game, int userId) {
    // Check rows.
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] == userId && game->board[i][1] == userId && game->board[i][2] == userId) {
            return true;
        }
    }

    // Check columns.
    for (int i = 0; i < 3; i++) {
        if (game->board[0][i] == userId && game->board[1][i] == userId && game->board[2][i] == userId) {
            return true;
        }
    }

    //check diagonals
    if (game->board[0][0] == userId && game->board[1][1] == userId && game->board[2][2] == userId) {
        return true;
    }

    if (game->board[0][2] == userId && game->board[1][1] == userId && game->board[2][0] == userId) {
        return true;
    }

    return false;
}

//check the outcome of the game 
GameOutcome Server::checkOutcome(std::shared_ptr<Game> game) {
    //use 1 won 
    if (didWin(game, game->user1)) {
        game->winnerId = game->user1;
        return GameOutcome::User1Win;
    }
    //user2 won 
    else if (didWin(game, game->user2)) {
        game->winnerId = game->user2;
        return GameOutcome::User2Win;
    }

    // Check for a tie.
    int emptyBoxes = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == 0) {
                emptyBoxes += 1;
            }
        }
    }

    return emptyBoxes == 0 ? GameOutcome::Tie : GameOutcome::Continue;
}

//see whos turn it is to play 
void Server::playTurn(std::shared_ptr<ClientContext> context) {
    int id = context->client.readInt();
    auto game = this->activeGames.at(id);

    int pos = context->client.readInt() - 1;
    int x = pos / 3;
    int y = pos % 3;
    std::cout << "PlayTurn: GameId=" << id << ", User: " << context->userId << ", Position: " << pos << std::endl;
    
    if (x < 0 || x > 2 || y < 0 || y > 2) {
        context->client.sendInt(GameOutcome::InvalidMove);
        context->client.sendGame(game);
        return;
    }

    if (game->board[x][y] != 0 || game->turnOfUser != context->userId) {
        context->client.sendInt(GameOutcome::InvalidMove);
        context->client.sendGame(game);
        return;
    }

    game->board[x][y] = context->userId;

    // Check if the game has concluded.
    auto outcome = checkOutcome(game);

    // Change the player.
    if (game->user1 == context->userId) {
        game->turnOfUser = game->user2;
    }
    else {
        game->turnOfUser = game->user1;
    }

    if (outcome == GameOutcome::User1Win || outcome == GameOutcome::User2Win) {
        outcome = context->userId == game->winnerId ? GameOutcome::YouWin : GameOutcome::YouLose;
    }

    context->client.sendInt(outcome);
    context->client.sendGame(game);
    return;
}

//used to see what the client would like to do 
int Server::processClient(std::shared_ptr<ClientContext> context)
{
    std::cout << "Processing request for client using thread " << context->client.getSd() << std::endl;

    try {
        while (true) {
            int command = context->client.readInt();
            switch (command) {
            case CommandType::RegisterUser:
                registerUser(context);
                break;

            case CommandType::StartNewGame:
                startNewGame(context);
                break;

            case CommandType::GetWaitingGames:
                getWaitingGames(context);
                break;

            case CommandType::JoinExistingGame:
                joinGame(context);
                break;

            case CommandType::IsGameStarted:
                isGameStarted(context);
                break;

            case CommandType::IsMyTurn:
                isMyTurn(context);
                break;

            case CommandType::PlayTurn:
                playTurn(context);
                break;

            default:
                std::cout << "Unknown command: " << command << std::endl;
                break;
            }
        }
    }
    catch (...) {
        std::cout << "Client connection terminated" << std::endl;
    }
}

//multithreading used by server so that multiple games can be played at once 
void* Server::threadProc(void* arg) {
    std::shared_ptr<ClientContext> context((ClientContext*)arg);
    context->server->processClient(context);
    return NULL;
}
