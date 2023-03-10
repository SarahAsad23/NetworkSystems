#pragma once

#include "SocketException.h"
#include "ByteBuffer.h"
#include "Game.h"
#include <memory>

class Socket {

protected:
	int sd;

public:

	Socket(int sd = -1) {
		this->sd = sd;
	}

	~Socket() {
		if (this->sd != -1) {
			close(this->sd);
			this->sd = -1;
		}
	}

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
