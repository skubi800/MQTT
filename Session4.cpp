#include "Session4.h"

// Public
Session4::Session4() {
	this->conn_ = nullptr;
}

Session4::~Session4() {
	cout << "Session removed!" << endl;
}

void Session4::addConnection(shared_ptr<MQTTConnection> conn) {
	this->conn_ = conn;
	conn->setSession(this);
}

shared_ptr<MQTTConnection> Session4::getConnection() {
	return conn_;
}

// Protected
void Session4::removeConnection() {
	this->conn_ = nullptr;
}
