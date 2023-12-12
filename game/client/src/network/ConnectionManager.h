#pragma once

#include <network/Client.h>

class ConnectionManager
{
public:
	ConnectionManager();

public:
	void connect();
	bool isConnected();
	void processReceivedMessages();
	void sendUpdate();
	void close();

private:
	network::Client _client;
	bool _active;
};