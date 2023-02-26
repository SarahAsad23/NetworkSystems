#include <iostream>

using namespace std;

class GameBoard
{
public:

	string player1; //first player 
	string player2; //second player 
	bool isStarted; //game started? 

	//used to print the 9x9 board 
	void buildBoard(char board[][3]) {
		cout << (" %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
		cout << ("-----------\n");
		cout << (" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
		cout << ("-----------\n");
		cout << (" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
	}

};