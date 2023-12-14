#pragma once

#include <network/Client.h>
#include <player/PlayerManager.h>

class ConnectionManager
{
public:
	ConnectionManager(PlayerManager& playerManager);

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
	PlayerManager& _playerManager;
	bool _active;
};