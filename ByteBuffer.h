#pragma once

class ByteBuffer {

public:
	uint8_t* buffer;
	size_t size;

	ByteBuffer(int size) {
		this->buffer = new uint8_t[size];
		if (this->buffer == NULL) {
			throw new std::exception();
		}

		this->size = size;
	}

	~ByteBuffer() {
		delete this->buffer;
	}
};
