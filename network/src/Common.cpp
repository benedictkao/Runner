#include "network/Common.h"

#include <cstdlib>

#include "logging/Logger.h"

int network::initENet()
{
	int result = enet_initialize();
	debug::log(
		result == 0 ? "[ENet] Successful ENet init!" : "[ENet] An error occurred when initializing ENet"
	);
	atexit(enet_deinitialize);
	return result;
}

network::Host::Host(ENetAddress* address, int maxConnections, int numChannels, int inBandwidth, int outBandwidth)
{
	_host = enet_host_create(
		address,									// the address to bind the server host to
		static_cast<size_t>(maxConnections),			// number of outgoing connections, server can connect to up to 32 clients
		static_cast<size_t>(numChannels),				// number of channels, 1 for now
		static_cast<enet_uint32>(inBandwidth),			// incoming bandwidth, 0 means infinite
		static_cast<enet_uint32>(outBandwidth)			// outgoing bandwidth, 0 means infinite
	);
	if (_host == NULL) {
		debug::log("[Host] Error occurred while trying to create an ENet host");
		// TODO: handle host not created error
	}
	else {
		debug::log("[Host] ENet host created successfully");
	}
}

network::Host::~Host()
{
	enet_host_destroy(_host);
}

ENetHost* network::Host::getHost() const
{
	return _host;
}

network::Connection::Connection(): _state(State::DISCONNECTED) {}

network::Connection::State network::Connection::getState()
{
	std::lock_guard lg(_mux);
	return _state;
}

network::Connection::State network::Connection::getRecentState() const
{
	return _state;
}

void network::Connection::setState(State state)
{
	std::lock_guard lg(_mux);
	_state = state;
}

int network::MessageService::send(ENetPeer* peer, const Buffer& buffer, enet_uint32 flags)
{
	ENetPacket* packet = createPacket(buffer, flags);
	return enet_peer_send(peer, 0, packet);
}

ENetPacket* network::MessageService::createPacket(const Buffer& buffer, enet_uint32 flags)
{
	auto data = buffer.data();
	auto size = buffer.size();
	return enet_packet_create(data, size, flags); // TODO: is this flag necessary?
}