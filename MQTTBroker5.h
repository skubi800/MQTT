#pragma once
#include "Common.h"
#include "MQTTPacket.h"
#include "MQTTConnection.h"
#include "MQTTBrokerVersion_I.h"

class MQTTBroker5 : public MQTTBrokerVersion_I {

public:
	//void processPacket(shared_ptr<MQTTPacket>, MQTTConnection*);
};