#pragma once
#include "Common.h"
#include "ServerTCP_I.h"

class TCPconnection : public enable_shared_from_this<TCPconnection> {

	boost::asio::io_context& io_context_;
	shared_ptr<boost::asio::ip::tcp::socket> tcp_socket_;

	unsigned int id_;
	ServerTCP_I* server_;

public:
	TCPconnection(boost::asio::io_context& context, shared_ptr<boost::asio::ip::tcp::socket> socket);
	virtual ~TCPconnection();
	
	bool connect(ServerTCP_I* server);
	void disconnect();
	bool isConnected() const;

	void closeSocket();

	shared_ptr<boost::asio::ip::tcp::socket> getSocket() const;
	
	unsigned int getId() const;
	void setId(unsigned int);

protected:
	virtual void onStartListening();
	virtual void onDisconnect();

	virtual void startReadData();
};