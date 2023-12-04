#pragma once

#include <enet/enet.h>

#include "ConcurrentQueue.h"
#include "Common.h"

namespace network
{
	struct InMessage {
		ENetPeer* source = nullptr;
		network::Buffer raw;
	};

	class EventReader
	{	

	public:
		template <typename _Connection, typename _Mutex>
		bool read(
			_Connection& connection,
			ConcurrentQueue<InMessage, _Mutex>& inQueue,
			int timeout, 
			TimeUnit& lastUpdated
		);

	private:
		inline ENetEvent* readNext(ENetHost* host, int timeout);

	private:
		ENetEvent _event;
	};
}

template <typename _Connection, typename _Mutex>
bool network::EventReader::read(
	_Connection& connection, 
	ConcurrentQueue<InMessage, _Mutex>& inQueue,
	int timeout, 
	TimeUnit& lastUpdated
)
{
	ENetHost* host = connection.getHost();
	while (auto event = readNext(host, timeout))
	{
		lastUpdated = network::currentTime();
		switch (event->type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				connection.onConnected(event);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				InMessage msg = { event->peer };
				msg.raw.fill(event->packet->data, event->packet->dataLength);
				inQueue.push(std::move(msg));
			}
				break;
			case ENET_EVENT_TYPE_DISCONNECT: 
			{
				if (connection.onDisconnected(event)) // return true means we should stop reading
					return true;
				break;
			}
			default:
				break;
		}
	}
	return false;
}

inline ENetEvent* network::EventReader::readNext(ENetHost* host, int timeout)
{
	return enet_host_service(host, &_event, timeout) > 0 ? &_event : nullptr;
}