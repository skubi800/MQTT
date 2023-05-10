#pragma once
#include "Common.h"
#include "Session_I.h"
#include "MQTTConnection.h"

class Session4 : public Session_I {

	shared_ptr<MQTTConnection> conn_;

public:
	Session4();
	~Session4();

	void addConnection(shared_ptr<MQTTConnection>);
	shared_ptr<MQTTConnection> getConnection();

protected:
	void removeConnection() override;
};