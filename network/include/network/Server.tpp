#include <logging/Logger.h>

template <typename _ConnectionCallback>
network::Server<_ConnectionCallback>::Server(int port, int maxConnections): _host(createHost(port, maxConnections)), _clients(maxConnections), _callback(nullptr) 
{
	static_assert(std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value, "Callback reference must be given for non-empty callback");
}

template <typename _ConnectionCallback>
network::Server<_ConnectionCallback>::Server(int port, int maxConnections, _ConnectionCallback& callback):
	_host(createHost(port, maxConnections)),
	_clients(maxConnections),
	_callback(&callback) 
{}

template <typename _ConnectionCallback>
inline network::Host network::Server<_ConnectionCallback>::createHost(int port, int maxConnections) const
{
	ENetAddress address { ENET_HOST_ANY, static_cast<enet_uint16>(port) };
	Host host(&address, maxConnections);
	return host;
}

template <typename _ConnectionCallback>
void network::Server<_ConnectionCallback>::onConnected(ENetEvent* event)
{
	debug::log( "[Server] A new client connected from %d:%d", event->peer->address.host, event->peer->address.port);
	_clients.insert(event->peer);
	if constexpr (!std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value)
		_callback->onConnected(event);
}

template <typename _ConnectionCallback>
bool network::Server<_ConnectionCallback>::onDisconnected(ENetEvent* event)
{
	debug::log( "[Server] Client %d:%d disconnected", event->peer->address.host, event->peer->address.port);
	_clients.erase(event->peer);
	event->peer->data = NULL;	// clean up the data
	if constexpr (!std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value)
		_callback->onDisconnected(event);
	return false;
}

template <typename _ConnectionCallback>
ENetHost* network::Server<_ConnectionCallback>::getHost() const
{
	return _host.getHost();
}

template <typename _ConnectionCallback>
ConcurrentQueue<network::InMessage>& network::Server<_ConnectionCallback>::getInQueue()
{
	return _readQueue;
}

template <typename _ConnectionCallback>
void network::Server<_ConnectionCallback>::run(int interval)
{
	EventReader reader;
	TimeUnit lastUpdated = network::currentTime();
	while (true)	// TODO: add stop mechanism?
	{
		reader.read(*this, _readQueue, interval, lastUpdated);
	}
}

template <typename _ConnectionCallback>
bool network::Server<_ConnectionCallback>::isConnected(ENetPeer* client)
{
	return _clients.contains(client);
}

template <typename _ConnectionCallback>
ConcurrentSet<ENetPeer*>& network::Server<_ConnectionCallback>::getClients()
{
	return _clients;
}