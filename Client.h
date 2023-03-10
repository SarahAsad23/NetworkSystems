
#pragma once

#include <map>
#include "Socket.h"

class Client : public Socket {

	int userId;

public: 
	Client() : userId(0) {
	}
	
	void connect(const char* server, int serverPort);
	void run();

private:

	void signUp();
	void listAvailableGames();
	void playGame(std::shared_ptr<Game> game);
	char getSymbol(std::shared_ptr<Game> game, int x, int y);
	std::shared_ptr<Game> waitForTurn(std::shared_ptr<Game> game);

	std::map<int, std::shared_ptr<Game>> getAllWaitingGames();
	std::shared_ptr<Game> joinExistingGame(int id);

	int startNewGame();
	void endGameIfConcluded(int outcome);

	void banner(const std::string& title);

};