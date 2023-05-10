#include "ServerTCP.h"


// Public
ServerTCP::ServerTCP(boost::asio::io_context& io_context, uint16_t port)
	: io_context_(io_context),
	tcp_acceptor_(make_shared<boost::asio::ip::tcp::acceptor>(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))),
	disconnectStrand_(make_shared<boost::asio::io_service::strand>(io_context)),
	socket_(make_shared<boost::asio::ip::tcp::socket>(io_context)),
	idCounter_(1) {}

ServerTCP::~ServerTCP() {
	stop();
}


bool ServerTCP::start() {
	acceptConnection();
	return true;
}

void ServerTCP::stop() {
	boost::system::error_code ec;
	tcp_acceptor_->cancel(ec);
}


// Protected
shared_ptr<TCPconnection> ServerTCP::setConnectionType(boost::asio::io_context& io_context, shared_ptr<boost::asio::ip::tcp::socket> socket) {
	return make_shared<TCPconnection>(io_context, std::move(socket));
}

void ServerTCP::onClientConnect(shared_ptr<TCPconnection> connection) {
	if (connection->getId() % 5000 == 0)
		cout << "[SERVER] Connection approved: " << connection->getId() << endl;
}

void ServerTCP::onClientDisconnect(shared_ptr<TCPconnection> con) {}

// Private
void ServerTCP::acceptConnection() {
	tcp_acceptor_->async_accept(*this->socket_, boost::asio::bind_executor(*disconnectStrand_.get(),
		[this](boost::system::error_code ec) {
			if (!ec) {
				shared_ptr<TCPconnection> newconn = setConnectionType(io_context_, socket_);

				while (connections_.count(idCounter_) != 0)
					++idCounter_;

				connections_.try_emplace(idCounter_, newconn);
				newconn->setId(idCounter_);

				onClientConnect(connections_[idCounter_]);
				connections_[idCounter_]->connect(this);

				++idCounter_;
				socket_ = make_shared<boost::asio::ip::tcp::socket>(io_context_);
			}
			else
				cout << "[SERVER] New Connection Error: " << ec.message() << " " << ec.what() << endl;
			
			acceptConnection();
		}));
}

void ServerTCP::removeConnection(unsigned int id) {
	boost::asio::dispatch(*disconnectStrand_.get(),
		[this, id]() {
			shared_ptr<TCPconnection> client = connections_[id];
			connections_.erase(id);
			client.reset();
		});
}

void ServerTCP::notifyDisconnectClient(unsigned int id) {
	onClientDisconnect(connections_[id]);
	removeConnection(id);
}