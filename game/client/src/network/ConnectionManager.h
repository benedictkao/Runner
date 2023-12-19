#pragma once

#include <common/Network.h>
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
	void close();

	template <common::messages::Type _type>
	void send();

	template <common::messages::Type _type, typename _Body>
	void send(const _Body&);

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