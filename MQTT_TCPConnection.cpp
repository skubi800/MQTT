#include "MQTT_TCPConnection.h"

// Public
MQTT_TCP_connection::MQTT_TCP_connection(boost::asio::io_context& context, shared_ptr<boost::asio::ip::tcp::socket> socket)
	: TCPconnection(context, socket) {
	queue_ = make_shared<Queue>(context, [this](shared_ptr<MQTTPacket> packet) {
		sendPacket(packet);
	});
}

MQTT_TCP_connection::~MQTT_TCP_connection() = default;

void MQTT_TCP_connection::disconnect() {
	MQTTConnection::disconnect();
	TCPconnection::disconnect();
}

void MQTT_TCP_connection::addPacketSendQueue(shared_ptr<MQTTPacket> packet) {
	queue_->push(packet);
}


// Protected
void MQTT_TCP_connection::onStartListening() {
	MQTTConnection::setConnection(dynamic_pointer_cast<MQTT_TCP_connection>(shared_from_this()));
}

void MQTT_TCP_connection::startReadData() {
	read2BytesHeader();
}


// Private
void MQTT_TCP_connection::read2BytesHeader() {
	MQTTConnection::packet = make_shared<MQTTPacket>();

	boost::asio::async_read(*TCPconnection::getSocket(), boost::asio::buffer(MQTTConnection::packet->fixedHeader_, 5), boost::asio::transfer_exactly(2),
		[this](const boost::system::error_code& ec, std::size_t bytes_received) {
			if (!ec) {
				MQTTConnection::packet->remainLength_ = MQTTConnection::packet->fixedHeader_[1] & 127;

				if (MQTTConnection::packet->fixedHeader_[1] & 128)
					readByteHeader();
				else
					readPayload();
			}
			else {
				//cout << "[" << TCPconnection::getId() << "]" << ec.what() << endl;
				disconnect();
			}
		});
}

void MQTT_TCP_connection::readByteHeader() {
	boost::asio::async_read(*TCPconnection::getSocket(), boost::asio::buffer(MQTTConnection::packet->fixedHeader_ + ++MQTTConnection::packet->position_, 5), boost::asio::transfer_exactly(1),
		[this](const std::error_code& ec, std::size_t bytes_received) {
			if (!ec) {
				MQTTConnection::packet->remainLength_ += (MQTTConnection::packet->fixedHeader_[MQTTConnection::packet->position_ + 1] & 127) * MQTTConnection::packet->multiplier_;
				MQTTConnection::packet->multiplier_ *= 128;

				if ((MQTTConnection::packet->multiplier_ == 128*128*128) && (MQTTConnection::packet->fixedHeader_[MQTTConnection::packet->position_] & 128)) {
					disconnect();
					return;
				}

				if (MQTTConnection::packet->fixedHeader_[MQTTConnection::packet->position_] & 128)
					readByteHeader();
				else
					readPayload();
			}
			else
				disconnect();
		});
}

void MQTT_TCP_connection::readPayload() {

	if (MQTTConnection::packet->remainLength_ > 0) {
		MQTTConnection::packet->payload_ = new uint8_t[MQTTConnection::packet->remainLength_];
	}
	else {
		processPacket();
		return;
	}

	boost::asio::async_read(*TCPconnection::getSocket(), boost::asio::buffer(MQTTConnection::packet->payload_, MQTTConnection::packet->remainLength_), boost::asio::transfer_exactly(MQTTConnection::packet->remainLength_),
		[this](const std::error_code& ec, std::size_t bytes_received) {
			if (!ec) {
				processPacket();
			}
			else
				disconnect();
		});
}

void MQTT_TCP_connection::sendPacket(shared_ptr<MQTTPacket> packet){
	boost::asio::async_write(*TCPconnection::getSocket(), boost::array<boost::asio::mutable_buffer, 2>{ boost::asio::buffer(packet->fixedHeader_, packet->fixedHeaderLength_), boost::asio::buffer(packet->payload_, packet->remainLength_) }, boost::asio::transfer_all(),
		[this, packet](const std::error_code& ec, std::size_t bytes_received) {
		if (!ec) {
			if(packet->callbackAfterSend_)
				packet->callbackAfterSend_();

			if (!queue_->isEmpty())
				queue_->pop();
			else
				queue_->setSending(false);
		}
		else
			cout << ec.message() << endl;
	});
}

void MQTT_TCP_connection::processPacket() {
	MQTTConnection::processPacket([this]() {
		read2BytesHeader();
	});
}