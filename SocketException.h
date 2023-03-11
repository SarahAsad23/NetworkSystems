#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* SocketException.h
*
* There are a lot of places where error checking needs to be done when creating the socket, 
* connecting to a host, binding, listening for incoming connections, and accepting connections. 
* SocketException contains one method that takes a string message and int error makes error 
* checking just a single line of code in the server and client 
*/


class SocketException {
	std::string message;
	int err;

public:
	SocketException(const std::string& message, int err = -1) {
		this->message = message;
		this->err = err;
	}
};
