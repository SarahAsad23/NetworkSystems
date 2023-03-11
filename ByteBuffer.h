#pragma once

/*
* Sarah Asad
* Term Project: Tic-Tac-Toe
* GameOutcomes.h
*
* this is used in the Socket class to do all the send and recieves in the cleint and server
*/


class ByteBuffer {

public:
	uint8_t* buffer; //buffer used to read and write 
	size_t size;     //size of the buffer 

	//constructor - sets size of the buffer 
	ByteBuffer(int size) {
		this->buffer = new uint8_t[size];
		if (this->buffer == NULL) {
			throw new std::exception();
		}

		this->size = size;
	}

	//destructor - deletes the buffer 
	~ByteBuffer() {
		delete this->buffer;
	}
};
