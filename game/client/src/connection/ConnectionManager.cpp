#include "ConnectionManager.h"

#include <logging/Logger.h>

static constexpr auto SERVER_ADDRESS { "127.0.0.1" };
static constexpr auto TIMEOUT{ common::PING_INTERVAL_MILLIS * 2 };

ConnectionManager::ConnectionManager(PlayerRepo& pRepo): 
	_client(SERVER_ADDRESS, common::PORT_NUMBER, *this),
	_active(false), 
	_pRepo(pRepo) 
{}

void ConnectionManager::connect()
{
	_active = true;
	while (_active)
	{
		bool expectedDisconnect = _client.connect(TIMEOUT);
		if (expectedDisconnect)
		{
			debug::log("[ConnectionManager] Connection ended");
			return;
		}
		else
		{
			debug::log("[ConnectionManager] Retry connection");
		}
	}
}

bool ConnectionManager::isConnected()
{
	return _client.isConnected();
}

void ConnectionManager::processReceivedMessages()
{
	auto& inQueue = _client.getInQueue();
	auto numMsgs = inQueue.size();
	for (int i = 0; i < numMsgs; ++i)
	{
		network::Buffer buffer = inQueue.popFront().raw;
		auto type = buffer.read<common::messages::Type>();
		switch (type) {
		case common::messages::Type::PING:
		{
			debug::log("[ConnectionManager] Server ping");
		}
		break;
		case common::messages::Type::PLAYER_JOIN:
		{
			auto payload = buffer.read<common::messages::JoinDetails>();
			_pRepo.onPlayerJoin(payload);
		}
		break;
		case common::messages::Type::PLAYER_UPDATE:
		{
			auto payload = buffer.read<common::messages::PlayerUpdate>();
			_pRepo.onPlayerUpdate(payload);
		}
		break;
		default:
		{
			debug::log("[ConnectionManager] Unknown message received: %d", type);
		}
		break;
		}
	}
}

void ConnectionManager::onConnected(ENetEvent* event)
{
	debug::log("[ConnectionManager] Connected!");
	send<common::messages::Type::PLAYER_JOIN>();
}

void ConnectionManager::onDisconnected(ENetEvent* event)
{
	debug::log("[ConnectionManager] Disconnected!");
}

void ConnectionManager::close()
{
	_active = false;
	_client.disconnect();
}