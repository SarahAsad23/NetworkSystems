#pragma once

#include <map>
#include <memory>
#include "Socket.h"
#include "GameOutcome.h"

class Server;

struct ClientContext {
	Server* server;
	Socket client;
	int userId;
	pthread_t threadId;

	ClientContext(Server* server, int sd) : 
		server(server), client(sd), threadId(0), userId(-1) {
	}
};

struct User {
	int id;
	std::string name;
};

class Server : public Socket {	

	int nextUserId = 100;
	int nextGameId = 5000;

	std::map<int, std::shared_ptr<User>> users;
	std::map<int, std::shared_ptr<Game>> activeGames;
	std::map<int, std::shared_ptr<Game>> waitingGames;

public: 
	Server()
	{
	}

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