#include "network/Server.h"

#include "logging/Logger.h"

network::Server::Server(int port, int maxConnections): _host(createHost(port, maxConnections)), _clients(maxConnections) {}

inline network::Host network::Server::createHost(int port, int maxConnections) const
{
	ENetAddress address { ENET_HOST_ANY, static_cast<enet_uint16>(port) };
	Host host(&address, maxConnections);
	return host;
}

void network::Server::onConnected(ENetEvent* event)
{
	debug::log( "[Server] A new client connected from %d:%d", event->peer->address.host, event->peer->address.port);
	_clients.insert(event->peer);
}

bool network::Server::onDisconnected(ENetEvent* event)
{
	debug::log( "[Server] Client %d:%d disconnected", event->peer->address.host, event->peer->address.port);
	_clients.erase(event->peer);
	event->peer->data = NULL;	// clean up the data
	return false;
}

ENetHost* network::Server::getHost() const
{
	return _host.getHost();
}

ConcurrentQueue<network::InMessage>& network::Server::getInQueue()
{
	return _readQueue;
}

void network::Server::run(int interval)
{
	EventReader reader;
	TimeUnit lastUpdated = network::currentTime();
	while (true)	// TODO: add stop mechanism?
	{
		reader.read(*this, _readQueue, interval, lastUpdated);
	}
}

bool network::Server::isConnected(ENetPeer* client)
{
	return _clients.contains(client);
}

ConcurrentSet<ENetPeer*>& network::Server::getClients()
{
	return _clients;
}

void network::Server::send(ENetPeer* client, const Buffer& buffer)
{
	// TODO: consider adding safety check that client is connected
	_msgService.send(client, buffer);
}