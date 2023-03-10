#include <iostream>
#include <cstdio>
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
#include <fstream>
#include <sstream>
#include "Client.h"
#include "CommandType.h"
#include "GameOutcome.h"

void Client::run()
{
	signUp();

	while (true) {
		std::cout << "(1) List available games" << std::endl;
		std::cout << "(2) Start new game" << std::endl;
		std::cout << "Choose: ";

		std::string input;
		std::cin >> input;

		if (input == "1") {
			listAvailableGames();
		}
		else if (input == "2") {
			startNewGame();
		}
	}
}

//setting up the client socket and connecting to the server
void Client::connect(const char *server, int serverPort) {

	struct hostent* host = gethostbyname(server);
	if (host == NULL) {
		throw new SocketException("Unable to resolve server name");
	}

	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));

	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr =
		inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSockAddr.sin_port = htons(serverPort);

	//create socket
	this->sd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sd < 0) {
		throw new SocketException("Error creating socket", this->sd);
	}

	//connect to server
	int result = ::connect(this->sd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
	if (result < 0) {
		throw new SocketException("Error connecting to server", result);
	}

	std::cout << "Connected to server" << std::endl;
}

//allows user to create a username
void Client::signUp() {
	std::cout << "Welcome to Tic-Tac-Toe\n";
	std::cout << "to begin, enter a username\n";
	std::cout << "Username: ";
	std::string username;
	std::cin >> username;

	sendInt(CommandType::RegisterUser);
	sendString(username);

	this->userId = readInt();
	std::cout << "Your User Id is " << userId << std::endl;
}

char Client::getSymbol(std::shared_ptr<Game> game, int x, int y) {
	int id = game->board[x][y];
	if (id == this->userId) {
		return 'O';
	}
	else if (id != 0) {
		return 'X';
	}

	return ' ';
}

void Client::playGame(std::shared_ptr<Game> game) {
	banner("Starting Game Play");

	while (true) {
		game = waitForTurn(game);

		std::cout << getSymbol(game, 0, 0) << " | " << getSymbol(game, 0, 1) << " | " << getSymbol(game, 0, 2) << std::endl;
		std::cout << ("---------\n");
		std::cout << getSymbol(game, 1, 0) << " | " << getSymbol(game, 1, 1) << " | " << getSymbol(game, 1, 2) << std::endl;
		std::cout << ("---------\n");
		std::cout << getSymbol(game, 2, 0) << " | " << getSymbol(game, 2, 1) << " | " << getSymbol(game, 2, 2) << std::endl;

		std::cout << "Enter your Move (1-9): ";
		std::string input;
		std::cin >> input;

		int pos = atoi(input.c_str());
		sendInt(CommandType::PlayTurn);
		sendInt(game->id);
		sendInt(pos);

		int outcome = readInt();
		game = readGame();
		endGameIfConcluded(outcome);
	}

	return;
}

void Client::endGameIfConcluded(int outcome) {

	std::string input; 

	switch (outcome) {
	case GameOutcome::YouWin:
		std::cout << "You Win!!" << std::endl;
		std::cout << "Enter 0 to unregister: " << std::endl;
		std::cin >> input;

		if (input == "0") {
			exit(0);
		}
		break;

	case GameOutcome::YouLose:
		std::cout << "You Lose!!" << std::endl;
		std::cout << "Enter 0 to unregister: " << std::endl;
		std::cin >> input;

		if (input == "0") {
			exit(0);
		}
		break;

	case GameOutcome::Tie:
		std::cout << "Tie!!" << std::endl;
		std::cout << "Enter 0 to unregister: " << std::endl;
		std::cin >> input;

		if (input == "0") {
			exit(0);
		}
		break;
	}

	
}

std::shared_ptr<Game> Client::waitForTurn(std::shared_ptr<Game> game) {
	std::cout << "Waiting for your turn: ";
	std::cout.flush();

	while (true) {
		sendInt(CommandType::IsMyTurn);
		sendInt(game->id);

		int outcome = readInt();
		int result = readInt();
		game = readGame();
		endGameIfConcluded(outcome);

		if (result > 0) {
			std::cout << std::endl;
			break;
		}

		std::cout << ".";
		std::cout.flush();
		sleep(1);
	}

	return game;
}

void Client::listAvailableGames() {
	auto games = getAllWaitingGames();
	if (games.size() == 0) {
		std::cout << "There are no available games. Start a new one instead." << std::endl;
		return;
	}

	banner("Displaying all available games");
	for (auto game : games) {
		std::cout << "Game " << game.second->id << ", Joined By: " << game.second->userName1 << std::endl;
	}

	std::cout << std::endl;
	std::cout << "Choose a Game Id or enter to return to main menu: ";

	std::string input;
	std::cin >> input;

	int selectedId = atoi(input.c_str());
	if (games.find(selectedId) == games.end()) {
		std::cout << "Invalid Game specified." << std::endl;
		return;
	}

	auto joinedGame = joinExistingGame(selectedId);
	playGame(joinedGame);
}

//prints out a list of all waiting games 
std::map<int, std::shared_ptr<Game>> Client::getAllWaitingGames() {
	sendInt(CommandType::GetWaitingGames);

	int total = readInt();
	std::map<int, std::shared_ptr<Game>> games;

	for (int i = 0; i < total; i++) {
		auto game = readGame();
		games.insert({ game->id, game });
	}

	return games;
}


std::shared_ptr<Game> Client::joinExistingGame(int id) {
	sendInt(CommandType::JoinExistingGame);
	sendInt(id);
	return readGame();
}

//allows a user to start a new game. to play they must wit for someone else to join 
int Client::startNewGame() {
	sendInt(CommandType::StartNewGame);
	auto game = readGame();
	std::cout << "New Game Started: " << game->id << ", waiting for second player to join: ";
	std::cout.flush();

	while (true) {
		sendInt(CommandType::IsGameStarted);
		sendInt(game->id);
		int result = readInt();
		if (result > 0) {
			std::cout << std::endl;
			break;
		}

		std::cout << ".";
		std::cout.flush();
		sleep(1);
	}

	playGame(game);
}

void Client::banner(const std::string& title) {
	std::cout << "------------------------------------------------------------------" << std::endl;
	std::cout << title << std::endl;
	std::cout << "------------------------------------------------------------------\n" << std::endl;
}