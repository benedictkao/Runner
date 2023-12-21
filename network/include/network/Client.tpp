#include <logging/Logger.h>

template <typename _ConnectionCallback>
network::Client<_ConnectionCallback>::Client(const char* serverName, int port):
	_client(NULL, 1),
	_server(nullptr),
	_callback(nullptr),
	_lastUpdated()
{
	static_assert(std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value, "Callback reference must be given for non-empty callback");
	enet_address_set_host(&_serverAddress, serverName);
	_serverAddress.port = port;
}

template <typename _ConnectionCallback>
network::Client<_ConnectionCallback>::Client(const char* serverName, int port, _ConnectionCallback& callback):
	_client(NULL, 1),
	_server(nullptr),
	_callback(&callback),
	_lastUpdated()
{
	enet_address_set_host(&_serverAddress, serverName);
	_serverAddress.port = port;
}

template <typename _ConnectionCallback>
bool network::Client<_ConnectionCallback>::connect(int timeout, int interval)
{
	if (_connection.getState() != Connection::State::DISCONNECTED)
		return false;
	
	_connection.setState(Connection::State::CONNECTING);
	debug::log("[Client] Attempting new connection to %d:%d...", _serverAddress.host, _serverAddress.port);

	if (_server)
		enet_peer_reset(_server);	// clear previous connection if there was any
	_server = enet_host_connect(
		_client.getHost(),
		&_serverAddress,
		1,			// number of channels, 1 for now
		0			// data to send on connect, nothing for now
	);
	if (_server == NULL) 
	{
		debug::log("[Client] Connection failed, no available peers for initiating an ENet connection.");
		_connection.setState(Connection::State::DISCONNECTED);
		return false;
	}

	// this is a blocking call, and should not return unless connection is broken
	readEvents(timeout, interval);

	bool expected = _connection.getState() == Connection::State::DISCONNECTING;
	resetConnection();
	return expected;
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::disconnect()
{
	debug::log("[Client] Manually disconnecting...");
	if (!isConnected())
	{
		resetConnection();
		return;
	}

	_connection.setState(Connection::State::DISCONNECTING);
	enet_peer_disconnect(
		_server,
		0		// data to send on disconnect, nothing for now
	);

	// no need to set disconnected state manually, disconnected event should be received in the read while loop.
	// If it is not, then it will still be updated after timeout anyway
}

template <typename _ConnectionCallback>
bool network::Client<_ConnectionCallback>::isConnected()
{
	return _connection.getState() == Connection::State::CONNECTED;
}

template <typename _ConnectionCallback>
network::Client<_ConnectionCallback>::InputQueue& network::Client<_ConnectionCallback>::getInQueue()
{
	return _readQueue;
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::onReceive(ENetEvent* event)
{
	_lastUpdated = currentTime();
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::onConnected(ENetEvent* event)
{
	debug::log("[Client] Connected to server!");
	_connection.setState(Connection::State::CONNECTED);
	_lastUpdated = currentTime();
	if constexpr (!std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value)
		_callback->onConnected(event);
}

template <typename _ConnectionCallback>
bool network::Client<_ConnectionCallback>::onDisconnected(ENetEvent* event)
{
	debug::log("[Client] Disconnected from server");
	if constexpr (!std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value)
		_callback->onDisconnected(event);
	return true;
}

template <typename _ConnectionCallback>
ENetHost* network::Client<_ConnectionCallback>::getHost() const
{
	return _client.getHost();
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::send(const Buffer& buffer)
{
	if (isConnected())
		MessageService::send(_server, buffer);
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::sendReliable(const Buffer& buffer)
{
	if (isConnected())
		MessageService::send(_server, buffer, ENET_PACKET_FLAG_RELIABLE);
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::readEvents(int timeout, int interval)
{
	EventReader reader;
	_lastUpdated = network::currentTime();
	while (_connection.getRecentState() != Connection::State::DISCONNECTED)
	{
		bool disconnected = reader.read(*this, _readQueue, interval);
		if (disconnected)
			return;

		auto now = network::currentTime();
		if (network::compareMillis(now, _lastUpdated, timeout))
		{
			debug::log("[Client] Connection timed out!");
			return;
		}
	}
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::resetConnection()
{
	enet_peer_reset(_server);
	_connection.setState(Connection::State::DISCONNECTED);
	debug::log("[Client] Connection reset");
}