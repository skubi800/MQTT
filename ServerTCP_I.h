#pragma once

class TCPconnection;

class ServerTCP_I {
private:
	virtual void notifyDisconnectClient(unsigned int id) = 0;

	friend TCPconnection;
};