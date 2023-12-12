#include "ConnectionManager.h"

//#include <thread>

#include <common/Network.h>
#include <logging/Logger.h>

static constexpr auto SERVER_ADDRESS { "127.0.0.1" };
static constexpr auto TIMEOUT{ common::PING_INTERVAL_MILLIS * 2 };

ConnectionManager::ConnectionManager(): _client(SERVER_ADDRESS, common::PORT_NUMBER), _active(false) {}

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
		common::messages::Type type;
		buffer.read(type);
		switch (type) {
		case common::messages::Type::PING:
		{
			debug::log("[ConnectionManager] Server ping");
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

void ConnectionManager::sendUpdate()
{

}

void ConnectionManager::close()
{
	_active = false;
	_client.disconnect();
}