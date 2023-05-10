#pragma once
#include "Common.h"

class MQTTPacket {
public:

	uint8_t* fixedHeader_;
	uint8_t* payload_;

	uint32_t remainLength_;
	uint8_t fixedHeaderLength_;

	unsigned int multiplier_;
	//unsigned short numberOfBytes;
	unsigned short position_;

	function<void()> callbackAfterSend_;

	MQTTPacket(bool initializeHeader = true);
	~MQTTPacket();

	uint8_t* getFixedHeader() {
		return fixedHeader_;
	}
	uint8_t* getPayload() {
		return payload_;
	}
	uint8_t getFixedHeaderLength() {
		return fixedHeaderLength_;
	}
	uint32_t getPayloadLength() {
		return remainLength_;
	}

	uint8_t getControlType(shared_ptr<MQTTPacket> packet) {
		return (packet->fixedHeader_[0] & 0xF0) >> 4;
	}

	uint8_t getReserved(shared_ptr<MQTTPacket> packet) {
		return packet->fixedHeader_[0] & 0x0F;
	}

	void allocateHeaderPayload(uint32_t size) {
		delete[] payload_;
		delete[] fixedHeader_;

		uint8_t fixedHeaderSize = 2;

		if (size >= 0 && size <= 127) {
			fixedHeaderSize = 2;
		}
		else if (size >= 128 && size <= 16383) {
			fixedHeaderSize = 3;
		}
		else if (size >= 16384 && size <= 2097151) {
			fixedHeaderSize = 4;
		}
		else if (size >= 2097152 && size <= 268435455) {
			fixedHeaderSize = 5;
		}

		fixedHeader_ = new uint8_t[fixedHeaderSize];
		fixedHeaderLength_ = fixedHeaderSize;

		payload_ = new uint8_t[size];
		remainLength_ = size;
		position_ = 1;

		do {
			fixedHeader_[position_] = size % 128;
			size /= 128;

			if (size > 0)
				fixedHeader_[position_] |= 128;
			position_++;
		} while (size > 0);
	}

	void setTypeAndReserved(uint8_t type, uint8_t reserved) {
		fixedHeader_[0] = (type << 4) + reserved;
	}
};
