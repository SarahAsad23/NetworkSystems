#include <cstdio>
#include <iostream>
#include <sys/types.h>		//socket, bind
#include <sys/socket.h>		//coket, bind, listen, inet_ntoa
#include <netinet/in.h>		//hton1, htons, inet_ntoa
#include <unistd.h>			//read, write, close 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Server.h"
#include "Client.h"
#include "GameBoard.h"

//helps user start the program - correct format 
void displayHelp() {
	cerr << "Usage: TicTacToe Server <Server-Port>" << endl;
	cerr << "                 Client <Server-Address> <Server-Port>" << endl;
	exit(1);
}

//prompts user to enter username 
void promptuser() {
	cout << "Welcome to Tic-Tac-Toe\n";
	cout << "to begin, enter a username\n";
	cout << "Username: ";
	string username;
	cin >> username;

	//call process request to see if user would liek to 
	//1. start a new game 
	//2. get a list of waiting games
	//3. join an existing game 
	
}

int main(int argc, char* argv[]) {

	if (argc < 3) {
		displayHelp();
	}

	if (strcmp(argv[1], "Server") == 0) {
		int serverPort = atoi(argv[2]);
		if (serverPort > 0) {
			Server server;
			server.Start(serverPort);
			return 0;
		}
	}
	else if (argc >= 4 && strcmp(argv[1], "Client") == 0) {
		int serverPort = atoi(argv[3]);
		if (serverPort > 0) {
			Client client;
			client.connect(argv[2], serverPort);
		}
	}

	displayHelp();

	promptuser();

}