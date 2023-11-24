#include "connection_handler.h"

network::Connection::Connection(ENetAddress* address, int maxConnections, int numChannels, int inBandwidth, int outBandwidth)
{
	host = enet_host_create(
		address,										// the address to bind the server host to
		static_cast<size_t>(maxConnections),			// number of outgoing connections, server can connect to up to 32 clients
		static_cast<size_t>(numChannels),				// number of channels, 1 for now
		static_cast<enet_uint32>(inBandwidth),			// incoming bandwidth, 0 means infinite
		static_cast<enet_uint32>(outBandwidth)			// outgoing bandwidth, 0 means infinite
	);
}

network::Connection::~Connection()
{
	enet_host_destroy(host);
}
