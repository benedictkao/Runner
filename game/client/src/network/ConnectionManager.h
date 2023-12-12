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

	/*
	* Interface methods, do not remove
	*/
	void onConnected(ENetEvent* event);
	void onDisconnected(ENetEvent* event);

private:
	network::Client<ConnectionManager> _client;
	bool _active;
};