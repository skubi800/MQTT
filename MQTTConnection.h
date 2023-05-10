#pragma once
#include "Common.h"
#include "MQTTPacket.h"
#include "MQTTBroker_I.h"
#include "Session_I.h"

class MQTTConnection {

	MQTTBroker_I* broker_;
	Session_I* session_;

	shared_ptr<MQTTConnection> thisConnection_;

	function<void(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>, function<void()>)> processingMethod_;
	
protected:
	shared_ptr<MQTTPacket> packet;

public:
	MQTTConnection();
	virtual ~MQTTConnection();

	void setBroker(MQTTBroker_I* broker);
	void setConnection(shared_ptr<MQTTConnection> conn);

	void setSession(Session_I* session);
	Session_I* getSession();

	void changeProcessingMethod(function<void(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>, function<void()>)>);
	void processPacket(function<void()>);

	virtual void addPacketSendQueue(shared_ptr<MQTTPacket>) {};

	virtual void disconnect();
};