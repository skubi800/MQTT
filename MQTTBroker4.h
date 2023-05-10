#pragma once
#include "Common.h"
#include "MQTTPacket.h"
#include "MQTTConnection.h"
#include "MQTTBrokerVersion_I.h"
#include "Session4.h"

class MQTTBroker4 : public MQTTBrokerVersion_I {

	unordered_map<string, shared_ptr<Session4>> sessions_;

	shared_mutex sessionsMutex_;
public:
	MQTTBroker4(boost::asio::io_service&);

private:
	void processPacketV4(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>, function<void()>&);
	void processSubscribtion(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>);
	void processUnSubscribtion(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>);

protected:
	void processConnectV4(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>, function<void()>&);

	uint16_t getTwoBytes(shared_ptr<MQTTPacket>, uint16_t position);

	shared_ptr<MQTTPacket> generateConnack(uint8_t code, bool sessionPresent = false);
	shared_ptr<MQTTPacket> generatePingResp();
	shared_ptr<MQTTPacket> generateSuback(uint16_t packetId, vector<uint8_t> listReturnCodes);
	shared_ptr<MQTTPacket> generateUnSuback(uint16_t packetId);

	bool authorizeV4(string, string);
};