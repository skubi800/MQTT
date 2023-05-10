#pragma once
#include "Common.h"

class MQTTPacket;
class MQTTConnection;

class MQTTBroker_I {
public:

	virtual void processConnect(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> connection, function<void()>& callback) {};
	virtual void processPacket(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> connection, function<void()>& callback) {};
};
