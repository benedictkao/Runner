#pragma once

#include <enet/enet.h>

namespace network
{
	class EventProcessor {
	public:
		virtual void process(const ENetEvent&);
	};

	enum Bandwidth: int {
		INFINITE = 0
	};

	class Connection {
	public:
		Connection(ENetAddress* address, int maxConnections, int numChannels = 1, int inBandwidth = Bandwidth::INFINITE, int outBandwidth = Bandwidth::INFINITE);
		~Connection();

	public:
		void read();

	private:
		ENetHost* host;
	};
}