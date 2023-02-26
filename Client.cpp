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

//setting up the client socket and connecting to the server
int Client::connect(const char *server, int serverPort) {

	struct hostent* host = gethostbyname(server);
	if (host == NULL) {
		cerr << "Cannot find hostname\n";
		return -1;
	}

	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));

	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr =
		inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSockAddr.sin_port = htons(serverPort);

	//create socket
	int clientSd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSd < 0) {
		cerr << "Cannot open a client TCP socket\n";
		return -1;
	}

	//connect to server
	int result = ::connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
	if (result < 0) {
		cerr << "Error Connecting\n";
		return -1;
	}

	cout << "Connected to server" << endl;
}

//prints out a list of all waiting games 
void Client::getAllWaitingGames() {

}

//decides which players turn it is 
int Client::getNextMove() {

}

int Client::joinExistingGame() {

}


//allows the current player to choose which position they would like to play 
int Client::makeMove() {

}

//allows a user to start a new game. to play they must wit for someone else to join 
int Client::startNewGame() {

}