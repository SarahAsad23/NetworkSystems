#pragma once

class SocketException {
	std::string message;
	int err;

public:
	SocketException(const std::string& message, int err = -1) {
		this->message = message;
		this->err = err;
	}
};
