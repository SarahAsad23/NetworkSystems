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
			client.run();
		}
	}

	displayHelp();
;
}