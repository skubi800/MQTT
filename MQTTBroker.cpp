#include "MQTTBroker.h"


// Public
MQTTbroker::MQTTbroker(boost::asio::io_context& asio_context)
	: ServerTCP(asio_context, 1883), MQTTBroker4(asio_context) {}

MQTTbroker::~MQTTbroker() {
	stop();
}

void MQTTbroker::startBroker() {
	cout << "[SERVER] Server starting" << endl;
	ServerTCP::start();
	cout << "[SERVER] Server successfully started" << endl;
}

void MQTTbroker::stopBroker() {
	ServerTCP::stop();
	cout << "[SERVER] Server is stopping" << endl;
}


void MQTTbroker::processConnect(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn, function<void()>& callback) {

	if (packet->getControlType(packet) != 1) {
		conn->disconnect();
		return;
	}

	if (packet->getReserved(packet) != 0) {
		conn->disconnect();
		return;
	}


	if (packet->remainLength_ < 2) {
		conn->disconnect();
		return;
	}		

	uint16_t protocolNameLength = MQTTBroker4::getTwoBytes(packet, 0);

	if (packet->remainLength_ < 2 + protocolNameLength) {
		conn->disconnect();
		return;
	}

	string protocolName = "";

	for (uint8_t i = 0; i < protocolNameLength; i++) {
		protocolName += packet->payload_[i + 2];
	}

	if (packet->remainLength_ < 2 + protocolNameLength + 1) {
		conn->disconnect();
		return;
	}

	uint8_t protocolLevel = packet->payload_[protocolNameLength + 2];

	cout << "Protcol Name: " << protocolName << endl;
	cout << "Protcol Level: " << (int)protocolLevel << endl;

	switch (protocolLevel) {
	case 3: {
		if(protocolName != "MQTsdp") {
			conn->disconnect();
			return;
		}

		break;
	}
	case 4: {
		if (protocolName != "MQTT") {
			conn->disconnect();
			return;
		}

		MQTTBroker4::processConnectV4(packet, conn, callback);

		break;
	}
	case 5: {
		if (protocolName != "MQTT") {
			conn->disconnect();
			return;
		}
		break;
	}
	default: {
		cout << "Unacceptable protocol" << endl;

		shared_ptr<MQTTPacket> connack = MQTTBroker4::generateConnack(0x01);
		connack->callbackAfterSend_ = [conn]() {
			conn->disconnect();
		};
		conn->addPacketSendQueue(connack);
		return;
	}
	}
}

// Protected
shared_ptr<TCPconnection> MQTTbroker::setConnectionType(boost::asio::io_context& io_context, shared_ptr<boost::asio::ip::tcp::socket> socket) {
	return make_shared<MQTT_TCP_connection>(io_context, std::move(socket));
}

void MQTTbroker::onClientConnect(shared_ptr<TCPconnection> connection) {
	MQTTConnection* ptr = dynamic_cast<MQTTConnection*>(connection.get());
	if (ptr)
		ptr->setBroker(this);
}