#include <logging/Logger.h>

template <typename _ConnectionCallback>
network::Client<_ConnectionCallback>::Client(const char* serverName, int port):
	_client(NULL, 1),
	_server(nullptr),
	_callback(nullptr)
{
	static_assert(std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value, "Callback reference must be given for non-empty callback");
	enet_address_set_host(&_serverAddress, serverName);
	_serverAddress.port = port;
}

template <typename _ConnectionCallback>
network::Client<_ConnectionCallback>::Client(const char* serverName, int port, _ConnectionCallback& callback):
	_client(NULL, 1),
	_server(nullptr),
	_callback(&callback)
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
void network::Client<_ConnectionCallback>::onConnected(ENetEvent* event)
{
	debug::log("[Client] Connected to server!");
	if constexpr (!std::is_same<_ConnectionCallback, EmptyConnectionCallback>::value)
		_callback->onConnected(event);
	_connection.setState(Connection::State::CONNECTED);
}

template <typename _ConnectionCallback>
bool network::Client<_ConnectionCallback>::onDisconnected(ENetEvent* event)
{
	debug::log("[Client] Disonnected to server");
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
		MessageService::send(_server, buffer);	// FIXME: should this be on a separate thread?
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::readEvents(int timeout, int interval)
{
	EventReader reader;
	auto lastUpdated = network::currentTime();
	while (_connection.getRecentState() != Connection::State::DISCONNECTED)
	{
		auto now = network::currentTime();
		auto timeSinceLastMsg = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdated);
		if (timeSinceLastMsg >= std::chrono::milliseconds(timeout))
		{
			debug::log("[Client] Connection timed out!");
			return;
		}

		bool disconnected = reader.read(*this, _readQueue, interval, lastUpdated);
		if (disconnected)
			return;
	}
}

template <typename _ConnectionCallback>
void network::Client<_ConnectionCallback>::resetConnection()
{
	enet_peer_reset(_server);
	_connection.setState(Connection::State::DISCONNECTED);
	debug::log("[Client] Connection reset");
}