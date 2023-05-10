#include "TCPConnection.h"


// Public
TCPconnection::TCPconnection(boost::asio::io_context& context, shared_ptr<boost::asio::ip::tcp::socket> socket)
	: io_context_(context),
	tcp_socket_(socket),
	id_(0),
	server_(nullptr) {}

TCPconnection::~TCPconnection() = default;


bool TCPconnection::connect(ServerTCP_I* server) {
	this->server_ = server;

	if (isConnected()) {
		onStartListening();
		startReadData();		
		return true;
	}
	return false;
}

void TCPconnection::disconnect() {
	if (isConnected()) {
		closeSocket();
		onDisconnect();
		server_->notifyDisconnectClient(id_);
	}
	else
		cout << "[" << TCPconnection::getId() << "]" << "Problem is here" << endl;
}

bool TCPconnection::isConnected() const {
	return tcp_socket_->is_open();
}

void TCPconnection::closeSocket() {
	boost::system::error_code ec;
	tcp_socket_->close(ec);
}

shared_ptr<boost::asio::ip::tcp::socket> TCPconnection::getSocket() const {
	return this->tcp_socket_;
}

unsigned int TCPconnection::getId() const { 
	return id_; 
}

void TCPconnection::setId(unsigned int id) {
	this->id_ = id;
}


// Protected
void TCPconnection::onStartListening() {}

void TCPconnection::onDisconnect() {}

void TCPconnection::startReadData() {}