#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Cleint.cpp 
* 
* The Client is built using TCP socket and connects to the server on the specified port. The client then promts the user to register. The user 
* then has the option to either get a list of existing/waiting games and from there they can join a waiting game - or the user can start a new 
* game. if the user joins a waiting game then the game is instatnly starts. if the user creates a new game then they need to wait until a seocnd 
* user joins for the game to start. From there each player takes turns marking their corresponding symbol in their desired box. After each turn 
* the client asks the server for the game outcome to see if either a player has won, if there is a tie, or if the game should contine. Each user 
* continues to take turns until there is a win or a tie. After the game is over, the outcome of the game is printed out and the user if prompted
* to enter 0 to unregister and exit the game. 
*
*
*/

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

//user is prompted to choose from 2 options - list games or start a new one 
void Client::run()
{
	//asked to register 
	signUp();

	//keep looping until user has made a valid choice 
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

//prompts a user register by entering a username
void Client::signUp() {
	std::cout << "Welcome to Tic-Tac-Toe\n";
	std::cout << "to begin, enter a username\n";
	std::cout << "Username: ";
	std::string username;
	std::cin >> username;
	
	//command type sent to the server
	sendInt(CommandType::RegisterUser);
	//username sent to the server 
	sendString(username);

	//generated ID is read from the server 
	this->userId = readInt();
	std::cout << "Your User Id is " << userId << std::endl;
}

//assignes an X or O to each user 
char Client::getSymbol(std::shared_ptr<Game> game, int x, int y) {
	int id = game->board[x][y];
	//if you are the second user joining then you are assigned an O
	if (id == this->userId) {
		return 'O';
	}
	//if you are the first user or user who created the game - assigned X
	else if (id != 0) {
		return 'X';
	}

	return ' ';
}

//this is where the game is started 
void Client::playGame(std::shared_ptr<Game> game) {
	banner("Starting Game Play");

	while (true) {
		//see which players turn it is 
		game = waitForTurn(game);

		//print out the board 
		std::cout << getSymbol(game, 0, 0) << " | " << getSymbol(game, 0, 1) << " | " << getSymbol(game, 0, 2) << std::endl;
		std::cout << ("---------\n");
		std::cout << getSymbol(game, 1, 0) << " | " << getSymbol(game, 1, 1) << " | " << getSymbol(game, 1, 2) << std::endl;
		std::cout << ("---------\n");
		std::cout << getSymbol(game, 2, 0) << " | " << getSymbol(game, 2, 1) << " | " << getSymbol(game, 2, 2) << std::endl;
		
		//prompt user for input - which block they would like to place their symbol on 
		std::cout << "Enter your Move (1-9): ";
		std::string input;
		std::cin >> input;

		int pos = atoi(input.c_str());
		//send the command tyoe to the server 
		sendInt(CommandType::PlayTurn);
		//send game ID so we know which game the move is beign played on 
		sendInt(game->id);
		//send the positon the move was played on 
		sendInt(pos);

		//read the outcoem of the game from the server 
		int outcome = readInt();
		//get the updated gameboard from the server 
		game = readGame();
		//if win or tie then call endGameifConcluded()
		endGameIfConcluded(outcome);
	}

	return;
}

//check to see if there is a win or a tie 
void Client::endGameIfConcluded(int outcome) {

	std::string input; 

	switch (outcome) {
	//player 1 win - also prompted to exit and unregister by entereing 0
	case GameOutcome::YouWin:
		std::cout << "You Win!!" << std::endl;
		std::cout << "Enter 0 to exit and unregister: " << std::endl;
		std::cin >> input;

		if (input == "0") {
			exit(0);
		}
		break;
	//player  win - also prompted to exit and unregister by entereing 0
	case GameOutcome::YouLose:
		std::cout << "You Lose!!" << std::endl;
		std::cout << "Enter 0 to exit and unregister: " << std::endl;
		std::cin >> input;

		if (input == "0") {
			exit(0);
		}
		break;
	//there is a tie - also prompted to exit and unregister by entereing 0
	case GameOutcome::Tie:
		std::cout << "Tie!!" << std::endl;
		std::cout << "Enter 0 to exit and unregister: " << std::endl;
		std::cin >> input;


		if (input == "0") {
			exit(0);
		}
		break;
	}

	
}

//see whos turn it is 
std::shared_ptr<Game> Client::waitForTurn(std::shared_ptr<Game> game) {
	std::cout << "Waiting for your turn: ";
	std::cout.flush();

	while (true) {
		//ask server if it is out turn 
		sendInt(CommandType::IsMyTurn);
		//also send game ID so serer knows which game we are asking about 
		sendInt(game->id);

		//read the outcome 
		int outcome = readInt();
		//read the result
		int result = readInt();
		//get the game
		game = readGame();
		//check to see if game over 
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

//lists out all avalable games a user can join 
void Client::listAvailableGames() {
	auto games = getAllWaitingGames();
	//if size == 0 then there are no game - user has to create a new game 
	if (games.size() == 0) {
		std::cout << "There are no available games. Start a new one instead." << std::endl;
		return;
	}

	//otherwise for loop and list out all the existing game data 
	banner("Displaying all available games");
	for (auto game : games) {
		std::cout << "Game " << game.second->id << ", Joined By: " << game.second->userName1 << std::endl;
	}

	//user joing ezisting game by entereing the game ID
	std::cout << std::endl;
	std::cout << "Choose a Game Id or enter to return to main menu: ";

	std::string input;
	std::cin >> input;

	//if game exists then user can join 
	int selectedId = atoi(input.c_str());
	if (games.find(selectedId) == games.end()) {
		std::cout << "Invalid Game specified." << std::endl;
		return;
	}
	//can now play 
	auto joinedGame = joinExistingGame(selectedId);
	playGame(joinedGame);
}

//prints out a list of all waiting games 
std::map<int, std::shared_ptr<Game>> Client::getAllWaitingGames() {
	//ask server for how many waiting games there are 
	sendInt(CommandType::GetWaitingGames);
	
	//read the number of total games  
	int total = readInt();
	std::map<int, std::shared_ptr<Game>> games;
	//insert the game ID and gameboard into map 
	for (int i = 0; i < total; i++) {
		auto game = readGame();
		games.insert({ game->id, game });
	}

	return games;
}

//user is able to join an existing game 
std::shared_ptr<Game> Client::joinExistingGame(int id) {
	//tell the server we want to join an existing game
	sendInt(CommandType::JoinExistingGame);
	//send the ID 
	sendInt(id);
	//get the game we want to join 
	return readGame();
}

//allows a user to start a new game. to play they must wait for someone else to join 
int Client::startNewGame() {
	//tell the server that we want to create a new game 
	sendInt(CommandType::StartNewGame);
	//get the game from the server
	auto game = readGame();
	std::cout << "New Game Started: " << game->id << ", waiting for second player to join: ";
	std::cout.flush();

	while (true) {
		//tell the server that the game has started
		sendInt(CommandType::IsGameStarted);
		//send the game ID 
		sendInt(game->id);
		//read resulkt from server 
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

//banner used for UI
void Client::banner(const std::string& title) {
	std::cout << "------------------------------------------------------------------" << std::endl;
	std::cout << title << std::endl;
	std::cout << "------------------------------------------------------------------\n" << std::endl;
}
