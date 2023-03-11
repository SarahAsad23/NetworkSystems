#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Socket.cpp
*
* handles all the sends and recieve communication by the 
* socket from the client and the server. 
*/


#include <cstdio>
#include <vector>
#include <iostream>
#include <memory>
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
#include <sstream>
#include <fstream>
#include <fcntl.h> 
#include <stdint.h>
#include "Socket.h"

//helper method
void Socket::receive(void* buffer, int size) {
	int bytesRead = 0;
	while (bytesRead < size) {
		unsigned char* offsetInBuffer = (unsigned char*)buffer + bytesRead;
		int count = recv(this->sd, offsetInBuffer, size - bytesRead, 0);
		if (count <= 0) {
			throw new SocketException("Error reciving data", count);
		}

		bytesRead += count;
	}
}

//reas an int 
int Socket::readInt() {
	int value;
	receive(&value, sizeof(value));
	return value;
}

//read a string 
std::string Socket::readString() {
	int length = 0;
	receive(&length, sizeof(length));

	ByteBuffer buffer(length);
	receive(buffer.buffer, length);

	return std::string((const char*)buffer.buffer, length);
}

//read a game 
std::shared_ptr<Game> Socket::readGame() {
	std::shared_ptr<Game> game = std::make_shared<Game>();

	game->id = readInt();
	game->user1 = readInt();
	game->userName1 = readString();

	game->user2 = readInt();
	game->userName2 = readString();

	game->turnOfUser = readInt();

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			game->board[i][j] = readInt();
		}
	}

	return game;
}

//send an int
void Socket::sendInt(int value) {
	int err = send(this->sd, &value, sizeof(value), 0);
	if (err < 0) {
		throw new SocketException("Error sending data", err);
	}
}

//send a string 
void Socket::sendString(const std::string& name) {
	int length = (int)name.length();
	sendInt(length);

	int err = send(this->sd, name.c_str(), length, 0);
	if (err < 0) {
		throw new SocketException("Error sending data", err);
	}
}

//send a game 
void Socket::sendGame(std::shared_ptr<Game> game) {
	sendInt(game->id);
	sendInt(game->user1);
	sendString(game->userName1);

	sendInt(game->user2);
	sendString(game->userName2);

	sendInt(game->turnOfUser);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			sendInt(game->board[i][j]);
		}
	}
}