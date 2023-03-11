#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Game.h
* 
* this file contains a struct Game which stores both players ID and usernames, the game board, 
* the game ID, and ID of the winning player. this is used by the server to generate games 
* 
*/

//sotres all the information for a game 
struct Game {
	int id;					//ID of the game 
		
	int user1;				//Id of player 1
	std::string userName1;	//username of player 1

	int user2;				//ID of player 2
	std::string userName2;	//username of player 2

	int turnOfUser;			//turn of the user - player 1 or 2 
	int board[3][3];		//3x3 game board
		
	int winnerId;			//ID of the player who has won the game 

	//default constructor - sets everything to zero and zeros out the game board 
	Game() :
		id(0), user1(0), user2(0), winnerId(0) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->board[i][j] = 0;
			}
		}
	}

	//constructor where both players are present 
	Game(int id, int userId, std::string name) :
		id(id), user1(userId), userName1(name), user2(-1), turnOfUser(userId) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->board[i][j] = 0;
			}
		}
	}
};
