#pragma once
#include "Common.h"
#include "MQTTBroker_I.h"
#include "TCPConnection.h"
#include "MQTT_TCPConnection.h"
#include "ServerTCP.h"
#include "MQTTBroker3.h"
#include "MQTTBroker4.h"
#include "MQTTBroker5.h"

class MQTTbroker : public MQTTBroker_I, protected ServerTCP,
public MQTTBroker3, public MQTTBroker4, public MQTTBroker5 {

public:
	MQTTbroker(boost::asio::io_context& asio_context);
	~MQTTbroker();

	void startBroker();
	void stopBroker();

	void processConnect(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>, function<void()>&) override;

protected:
	shared_ptr<TCPconnection> setConnectionType(boost::asio::io_context& asio_context, shared_ptr<boost::asio::ip::tcp::socket> socket) override;
	void onClientConnect(shared_ptr<TCPconnection>) override;
};
