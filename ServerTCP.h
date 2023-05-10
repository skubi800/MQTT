#include "Common.h"
#include "ServerTCP_I.h"
#include "TCPConnection.h"
#include "MQTT_TCPConnection.h"

class ServerTCP : public ServerTCP_I {

	boost::asio::io_context& io_context_;
	shared_ptr<boost::asio::ip::tcp::acceptor> tcp_acceptor_;

	unordered_map<unsigned int, shared_ptr<TCPconnection>> connections_;
	shared_ptr<boost::asio::io_service::strand> disconnectStrand_;
	shared_ptr<boost::asio::ip::tcp::socket> socket_;
	unsigned int idCounter_;

public:
	ServerTCP(boost::asio::io_context& io_context, uint16_t port);
	virtual ~ServerTCP();

	bool start();
	void stop();

protected:
	virtual shared_ptr<TCPconnection> setConnectionType(boost::asio::io_context& asio_context, shared_ptr<boost::asio::ip::tcp::socket> socket);
	virtual void onClientConnect(shared_ptr<TCPconnection> connection);
	virtual void onClientDisconnect(shared_ptr<TCPconnection> connection);

private:
	void acceptConnection();
	void removeConnection(unsigned int id);

	void notifyDisconnectClient(unsigned int id) override;
};