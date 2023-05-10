#include "MQTTPacket.h"


// Public

MQTTPacket::MQTTPacket(bool initializeHeader) {
	payload_ = nullptr;
	if (initializeHeader)
		fixedHeader_ = new uint8_t[5];
	else
		fixedHeader_ = nullptr;

	callbackAfterSend_ = nullptr;

	remainLength_ = 0;
	multiplier_ = 1;

	position_ = 1;
}

MQTTPacket::~MQTTPacket() {
	delete[] fixedHeader_;
	delete[] payload_;
}