#pragma once

#include <common/Network.h>
#include <network/Client.h>
#include <player/PlayerRepo.h>

class ConnectionManager
{
public:
	ConnectionManager(PlayerRepo& playerRepo);

public:
	void connect();
	bool isConnected();
	void processReceivedMessages();
	void close();

	template <common::messages::Type _type>
	void send()
	{
		network::BufferWriter<common::messages::Type> writer;
		_client.send(writer.writeToBuffer<_type>());
	}

	template <common::messages::Type _type, typename _Body>
	void send(const _Body& body)
	{
		network::BufferWriter<common::messages::Type> writer;
		_client.send(writer.writeToBuffer<_type>(body));
	}

	/*
	* Interface methods, do not remove
	*/
	void onConnected(ENetEvent* event);
	void onDisconnected(ENetEvent* event);

private:
	network::Client<ConnectionManager> _client;
	PlayerRepo& _pRepo;
	bool _active;
};