#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Main.cpp
* 
* This is where the Client and Server connect() functions are called. The Server is powered on and waits for a connection 
* and the Client(s) are able to connect. There is also a displayHelp() method added in to help a user understand how to start 
* Client/Server if they are having difficulties starting the program. If the command line recieves 3 arguments then it knows 
* that it is the Server. if the command line recieves 4 arguments then it knows that it is the Client, 
*
*
*/


#include <cstdio>
#include <iostream>
#include <sys/types.h>		//socket, bind
#include <sys/socket.h>		//coket, bind, listen, inet_ntoa
#include <netinet/in.h>		//hton1, htons, inet_ntoa
#include <unistd.h>			//read, write, close 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Socket.h"
#include "Server.h"
#include "Client.h"

//helps user start the program - correct format 
void displayHelp() {
	std::cerr << "Usage: TicTacToe Server <Server-Port>" << std::endl;
	std::cerr << "                 Client <Server-Address> <Server-Port>" << std::endl;
	exit(1);
}

int main(int argc, char* argv[]) {

	//if less than 3 arguments recieved then display help 
	if (argc < 3) {
		displayHelp();
	}

	//if second argument is "Server" then it is the server running 
	if (strcmp(argv[1], "Server") == 0) {
		int serverPort = atoi(argv[2]);
		if (serverPort > 0) {
			Server server;
			server.Start(serverPort);
			return 0;
		}
	}
	//if second argument is "Client" then it is the Client connecting
	//call client.connect() and client.run()
	else if (argc >= 4 && strcmp(argv[1], "Client") == 0) {
		int serverPort = atoi(argv[3]);
		if (serverPort > 0) {
			Client client;
			client.connect(argv[2], serverPort);
			client.run();
		}
	}

	displayHelp();
;
}