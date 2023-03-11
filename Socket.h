#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* Socket.h
*
* This file contains all the method definitiosn for Socket.cpp. It handles all the sends and recieve communication by the 
* socket from the client and the server. 
*/

#include "SocketException.h"
#include "ByteBuffer.h"
#include "Game.h"
#include <memory>

class Socket {

protected:
	int sd;

public:
	//constructor 
	Socket(int sd = -1) {
		this->sd = sd;
	}
	//desctructoe = closes the socket 
	~Socket() {
		if (this->sd != -1) {
			close(this->sd);
			this->sd = -1;
		}
	}

	//helper method for testing - checking socket value 
	int getSd() {
		return this->sd;
	}

	void receive(void* buffer, int size);

	int readInt();
	std::string readString();
	std::shared_ptr<Game> readGame();

	void sendInt(int value);
	void sendString(const std::string& name);
	void sendGame(std::shared_ptr<Game> game);
};
