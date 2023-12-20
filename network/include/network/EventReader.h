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
			int timeout
		);

	private:
		inline ENetEvent* readNext(ENetHost* host, int timeout);

	private:
		ENetEvent _event;
	};
}

#include "EventReader.tpp"