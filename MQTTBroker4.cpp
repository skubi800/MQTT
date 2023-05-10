#include "MQTTBroker4.h"


// Public
MQTTBroker4::MQTTBroker4(boost::asio::io_service& asio_context) {}

// Private

void MQTTBroker4::processPacketV4(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn, function<void()>& callback) {
	switch ((packet->fixedHeader_[0] & 0xF0) >> 4) {
	case 8: {
		processSubscribtion(packet, conn);
		break;
	}
	case 10: {
		processUnSubscribtion(packet, conn);
		break;
	}
	case 12: {
		conn->addPacketSendQueue(MQTTBroker4::generatePingResp());
		break;
	}
	case 14: {
		conn->disconnect();
		return;
	}
	}

	callback();
}

void MQTTBroker4::processSubscribtion(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn) {
	if ((packet->fixedHeader_[0] & 0x0F) != 2) {
		conn->disconnect();
		return;
	}

	uint16_t position = 2;
	uint16_t length = 0;
	string topic = "";
	uint8_t qos = 0;
	vector<uint8_t> listReturnCodes;

	if (packet->remainLength_ < position) {
		conn->disconnect();
		return;
	}
	uint16_t packetId = getTwoBytes(packet, position - 2);

	while (packet->remainLength_ > position) {
		position += 2;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}
		
		length = getTwoBytes(packet, position - 2);
		if (packet->remainLength_ < position + length) {
			conn->disconnect();
			return;
		}

		topic = "";
		for (int i = 0; i < length; i++) {
			topic += packet->payload_[i + position];
		}
		position += length + 1;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}
		
		qos = packet->payload_[position - 1];
		cout << "Subscribed topic: " << topic << endl;
		listReturnCodes.push_back(qos);
	}
	
	if (packet->remainLength_ < position) {
		conn->disconnect();
		return;
	}
	
	conn->addPacketSendQueue(generateSuback(packetId, listReturnCodes));
}

void MQTTBroker4::processUnSubscribtion(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn) {
	if ((packet->fixedHeader_[0] & 0x0F) != 2) {
		conn->disconnect();
		return;
	}

	uint16_t position = 2;
	uint16_t length = 0;
	string topic = "";

	if (packet->remainLength_ < position) {
		conn->disconnect();
		return;
	}
	uint16_t packetId = getTwoBytes(packet, position - 2);

	while (packet->remainLength_ > position) {
		position += 2;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}

		length = getTwoBytes(packet, position - 2);
		if (packet->remainLength_ < position + length) {
			conn->disconnect();
			return;
		}

		topic = "";
		for (int i = 0; i < length; i++) {
			topic += packet->payload_[i + position];
		}
		position += length;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}

		cout << "Unsubscribed topic: " << topic << endl;
	}

	if (packet->remainLength_ < position) {
		conn->disconnect();
		return;
	}

	conn->addPacketSendQueue(generateUnSuback(packetId));
}


// Protected
void MQTTBroker4::processConnectV4(shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn, function<void()>& callback) {
	uint16_t position = 10;
	
	if (packet->remainLength_ < position) {
		conn->disconnect();
		return;
	}

	bool usernameFlag = packet->payload_[7] & 0x80;
	bool passwordFlag = packet->payload_[7] & 0x40;
	bool willRetain = packet->payload_[7] & 0x20;
	uint8_t willQos = (packet->payload_[7] & 0x10 >> 3) + (packet->payload_[7] & 0x08 >> 3);
	bool willFlag = packet->payload_[7] & 0x04;
	bool cleanSession = packet->payload_[7] & 0x02;
	bool Reserved = packet->payload_[7] & 0x01;

	if (Reserved != 0) {
		conn->disconnect();
		return;
	}

	if (willFlag) {
		if (willQos == 3) {
			conn->disconnect();
			return;
		}
	}
	else {
		if (willQos != 0) {
			conn->disconnect();
			return;
		}

		if (willRetain != 0) {
			conn->disconnect();
			return;
		}
	}

	if (usernameFlag == false) {
		if (passwordFlag != false) {
			conn->disconnect();
			return;
		}
	}

	uint16_t keepAlive = getTwoBytes(packet, position - 2);	
	string clientId = "";
	string willTopic = "";
	string willMessage = "";
	string username = "";
	string password = "";

	uint16_t length = 0;

	position += 2;
	if (packet->remainLength_ < position) {
		conn->disconnect();
		return;
	}

	length = getTwoBytes(packet, position - 2);
	if (packet->remainLength_ < position + length) {
		conn->disconnect();
		return;
	}	

	for (uint16_t i = 0; i < length; i++) {
		clientId += packet->payload_[i + position];
	}

	position += length;
	cout << "Client id: " << clientId << endl;

	if (clientId.length() == 0) {
		shared_ptr<MQTTPacket> connack = generateConnack(0x02);
		connack->callbackAfterSend_ = [conn]() {
			conn->disconnect();
		};
		conn->addPacketSendQueue(connack);
		return;
	}

	if (willFlag) {
		position += 2;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}

		length = getTwoBytes(packet, position - 2);
		if (packet->remainLength_ < position + length) {
			conn->disconnect();
			return;
		}

		for (uint16_t i = 0; i < length; i++) {
			willTopic += packet->payload_[i + position];
		}
		position += length;
		
		cout << "Will Topic: " << willTopic << endl;

		position += 2;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}

		length = getTwoBytes(packet, position - 2);
		if (packet->remainLength_ < position + length) {
			conn->disconnect();
			return;
		}

		for (uint16_t i = 0; i < length; i++) {
			willMessage += packet->payload_[i + position];
		}
		position += length;

		cout << "Will message: " << willMessage << endl;
	}

	if (usernameFlag) {
		position += 2;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}

		length = getTwoBytes(packet, position - 2);
		if (packet->remainLength_ < position + length) {
			conn->disconnect();
			return;
		}

		for (uint16_t i = 0; i < length; i++) {
			username += packet->payload_[i + position];
		}
		position += length;
		cout << "Username: " << username << endl;
	}

	if (passwordFlag) {
		position += 2;
		if (packet->remainLength_ < position) {
			conn->disconnect();
			return;
		}

		length = getTwoBytes(packet, position - 2);
		if (packet->remainLength_ < position + length) {
			conn->disconnect();
			return;
		}

		for (uint16_t i = 0; i < length; i++) {
			password += packet->payload_[i + position];
		}
		position += length;

		cout << "Password: " << password << endl;
	}

	if (position != packet->remainLength_) {
		conn->disconnect();
		return;
	}


	// Processing gathered data	

	if (authorizeV4(username, password) != true) {
		conn->disconnect();
		return;
	}
	
	conn->changeProcessingMethod([this](shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn, function<void()> callback) {
		processPacketV4(packet, conn, callback);
	});


	bool sessionPresent = false;
	{
		unique_lock lock(sessionsMutex_);

		if (sessions_.find(clientId) != sessions_.end())
			sessionPresent = true;

		shared_ptr<Session4> session = nullptr;
		if (cleanSession) {
			if (sessionPresent) {
				session = sessions_[clientId];
			}
			else
				session = make_shared<Session4>();

			shared_ptr<MQTTConnection> tempConn = session->getConnection();
			if (tempConn)
				tempConn->disconnect();
			
			session->addConnection(conn);
			sessions_.insert({ move(clientId), session });
			sessionPresent = false;
		}
		else if (sessionPresent) {
			session = sessions_[clientId];
			shared_ptr<MQTTConnection> tempConn = session->getConnection();
			if (tempConn)
				tempConn->disconnect();
			session->addConnection(conn);
		}
		else {
			session = make_shared<Session4>();
			session->addConnection(conn);
			sessions_.insert({ move(clientId), session });
		}
	}

	conn->addPacketSendQueue(generateConnack(0x00, sessionPresent));
	callback();		
}

uint16_t MQTTBroker4::getTwoBytes(shared_ptr<MQTTPacket> packet, uint16_t position) {
	return (packet->payload_[position] << 8) + packet->payload_[position + 1];
}

shared_ptr<MQTTPacket> MQTTBroker4::generateConnack(uint8_t code, bool sessionPresent) {
	shared_ptr<MQTTPacket> connack = make_shared<MQTTPacket>(false);

	connack->allocateHeaderPayload(2);
	connack->setTypeAndReserved(2, 0);
	connack->payload_[0] = sessionPresent;
	connack->payload_[1] = code;

	return connack;
}

shared_ptr<MQTTPacket> MQTTBroker4::generatePingResp() {
	shared_ptr<MQTTPacket> pingResp = make_shared<MQTTPacket>(false);
	pingResp->allocateHeaderPayload(0);
	pingResp->setTypeAndReserved(13, 0);
	return pingResp;
}

shared_ptr<MQTTPacket> MQTTBroker4::generateSuback(uint16_t packetId, vector<uint8_t> listReturnCodes) {
	shared_ptr<MQTTPacket> suback = make_shared<MQTTPacket>(false);
	suback->allocateHeaderPayload(2 + listReturnCodes.size());
	suback->setTypeAndReserved(9, 0);

	suback->payload_[0] = ((packetId & 0xFF00) >> 8);
	suback->payload_[1] = packetId & 0x00FF;
	for (unsigned short i = 0; i < listReturnCodes.size(); i++) {
		suback->payload_[2 + i] = listReturnCodes[i];
	}
	return suback;
}

shared_ptr<MQTTPacket> MQTTBroker4::generateUnSuback(uint16_t packetId) {
	shared_ptr<MQTTPacket> unsuback = make_shared<MQTTPacket>(false);
	unsuback->allocateHeaderPayload(2);
	unsuback->setTypeAndReserved(11, 0);

	unsuback->payload_[0] = ((packetId & 0xFF00) >> 8);
	unsuback->payload_[1] = packetId & 0x00FF;

	return unsuback;
}

bool MQTTBroker4::authorizeV4(string username, string password) {
	return true;
}
