#include "MQTTConnection.h"

// Public
MQTTConnection::MQTTConnection() 
	: broker_(nullptr),
	session_(nullptr),
	thisConnection_(nullptr),
	packet(nullptr) {

	processingMethod_ = [this](shared_ptr<MQTTPacket> packet, shared_ptr<MQTTConnection> conn, function<void()> callback) {
		broker_->processConnect(packet, conn, callback);
	};
}

MQTTConnection::~MQTTConnection() {
	cout << "Removed" << endl;
}


void MQTTConnection::setBroker(MQTTBroker_I* broker) {
	this->broker_ = broker;
}

void MQTTConnection::setConnection(shared_ptr<MQTTConnection> conn) {
	this->thisConnection_ = conn;
}

void MQTTConnection::setSession(Session_I* session) {
	this->session_ = session;
}

Session_I* MQTTConnection::getSession() {
	return this->session_;
}


void MQTTConnection::changeProcessingMethod(function<void(shared_ptr<MQTTPacket>, shared_ptr<MQTTConnection>, function<void()>)> function) {
	this->processingMethod_ = function;
}

void MQTTConnection::processPacket(function<void()> callback) {
	processingMethod_(packet, thisConnection_, callback);
}

void MQTTConnection::disconnect() {
	this->thisConnection_ = nullptr;
	if (session_) {
		this->session_->removeConnection();
	}
}