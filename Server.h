#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Server.h
*
* This file contains all the method definitions for Server.cpp
*/

#include <map>
#include <memory>
#include "Socket.h"
#include "GameOutcome.h"

class Server;

struct ClientContext {
	Server* server;		//Server
	Socket client;		//Client socket 
	int userId;			//USer ID
	pthread_t threadId;	//thread

	//constructor sets server, slient socket, threadID to 0, and userID to -1
	ClientContext(Server* server, int sd) : 
		server(server), client(sd), threadId(0), userId(-1) {
	}
};

//Struct used to store all information about a player 
struct User {
	int id;				//the players ID
	std::string name;	//the username associated with each user 
};

class Server : public Socket {	

	int nextUserId = 100;	//user ID is generated for each user by incrementing the initial value 100
	int nextGameId = 5000;	//game ID is generated for each game by incrementing the initial value 5000

	std::map<int, std::shared_ptr<User>> users;			//stores all the players 
	std::map<int, std::shared_ptr<Game>> activeGames;	//stores all the active games - games ebign played
	std::map<int, std::shared_ptr<Game>> waitingGames;  //stores all the waiting games - games waiting for a second player ot join 

public: 
	//constructor  
	Server()
	{
	}

	//destructor 
	~Server() 
	{
	}

	void Start(int port);

private:
	void registerUser(std::shared_ptr<ClientContext> context);
	void startNewGame(std::shared_ptr<ClientContext> context);
	void getWaitingGames(std::shared_ptr<ClientContext> context);
	void joinGame(std::shared_ptr<ClientContext> context);
	void isGameStarted(std::shared_ptr<ClientContext> context);
	void isMyTurn(std::shared_ptr<ClientContext> context);
	void playTurn(std::shared_ptr<ClientContext> context);
	void Findwinner(std::shared_ptr<ClientContext> context);
	int processClient(std::shared_ptr<ClientContext> context);
	bool didWin(std::shared_ptr<Game> game, int userId);
	GameOutcome checkOutcome(std::shared_ptr<Game> game);
	static void* threadProc(void* arg);
};