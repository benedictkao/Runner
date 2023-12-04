#include "network/Common.h"

#include "logging/Logger.h"

network::Host::Host(const char* hostName, int port, int maxConnections, int numChannels, int inBandwidth, int outBandwidth)
{
	ENetAddress address;
	enet_address_set_host(&address, hostName);
	address.port = port;
	_host = enet_host_create(
		&address,										// the address to bind the server host to
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

network::Host::Host(int maxConnections, int numChannels, int inBandwidth, int outBandwidth)
{
	_host = enet_host_create(
		NULL,											// the address to bind the server host to
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

void network::Connection::setState(State state)
{
	std::lock_guard lg(_mux);
	_state = state;
}