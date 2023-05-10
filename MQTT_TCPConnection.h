#pragma once
#include "Common.h"
#include "TCPConnection.h"
#include "MQTTConnection.h"

class Queue {
	shared_ptr<boost::asio::io_service::strand> queueStrand_;
	deque<shared_ptr<MQTTPacket>> queue_;
	
	function<void(shared_ptr<MQTTPacket>)> callback_;

	bool isSending;
public: 
	Queue(boost::asio::io_context& context, function<void(shared_ptr<MQTTPacket>)> callback) {
		queueStrand_ = make_shared<boost::asio::io_service::strand>(context);
		callback_ = callback;
		isSending = false;
	}

	void push(shared_ptr<MQTTPacket> packet) {
		boost::asio::dispatch(*queueStrand_.get(), [this, packet]() {
			queue_.push_back(packet);
			if (!isSending)
				pop();
		});
	}

	void pop() {
		boost::asio::dispatch(*queueStrand_.get(), [this]() {
			shared_ptr<MQTTPacket> packet = queue_.front();
			queue_.pop_front();
			callback_(packet);
		});
	}

	void setSending(bool sending) {
		isSending = sending;
	}

	bool isEmpty() {
		return queue_.empty();
	}
};

class MQTT_TCP_connection : public TCPconnection, public MQTTConnection {
	shared_ptr<Queue> queue_;

public:
	MQTT_TCP_connection(boost::asio::io_context& context, shared_ptr<boost::asio::ip::tcp::socket> socket);
	virtual ~MQTT_TCP_connection();

	void disconnect();

	void addPacketSendQueue(shared_ptr<MQTTPacket>) override;

protected:
	void onStartListening() override;
	void startReadData() override;

private:

	void read2BytesHeader();
	void readByteHeader();
	void readPayload();

	void sendPacket(shared_ptr<MQTTPacket> packet);

	void processPacket();
};