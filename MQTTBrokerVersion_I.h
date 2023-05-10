#pragma once
#include "Common.h"
#include "MQTTPacket.h"

class MQTTConnection;

class MQTTBrokerVersion_I {

public:
	//virtual void processPacket(shared_ptr<MQTTPacket>, MQTTConnection*) = 0;
	//virtual void processConnect(shared_ptr<MQTTPacket>, MQTTConnection*) = 0;
};