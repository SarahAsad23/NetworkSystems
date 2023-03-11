#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Client.h
*
* This file contains all the method definitions for Client.cpp
*/

#include <map>
#include "Socket.h"

class Client : public Socket {

	int userId; //ID associated with each user 

public: 
	
	//constructor - intitially sets the userID to 0 
	Client() : userId(0) {
	}
	
	void connect(const char* server, int serverPort);
	void run();

private:

	void signUp();
	void listAvailableGames();
	void playGame(std::shared_ptr<Game> game);
	char getSymbol(std::shared_ptr<Game> game, int x, int y);
	std::shared_ptr<Game> waitForTurn(std::shared_ptr<Game> game); //sotres the game and user whos current tirn it is 

	std::map<int, std::shared_ptr<Game>> getAllWaitingGames(); //stores all the games that are waiting for a second player to join 
	std::shared_ptr<Game> joinExistingGame(int id);

	int startNewGame();
	void endGameIfConcluded(int outcome);

	void banner(const std::string& title);

};