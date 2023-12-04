template <typename _MsgType>
network::Client<_MsgType>::Client(const ENetAddress& server): _client(NULL, 1), _server(nullptr), _serverAddress(server) {}

template <typename _MsgType>
bool network::Client<_MsgType>::connect(int timeout, int interval)
{
	if (_connection.getState() != Connection::State::DISCONNECTED)
		return false;
	
	_connection.setState(Connection::State::CONNECTING);
	std::cout << "[Client] Attempting new connection to " << _serverAddress.host << ':' << _serverAddress.port << "..." << std::endl;

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
		std::cout << "[Client] Connection failed, no available peers for initiating an ENet connection." << std::endl;
		_connection.setState(Connection::State::DISCONNECTED);
		return false;
	}

	// this is a blocking call, and should not return unless connection is broken
	readEvents(timeout, interval);

	bool expected = _connection.getState() == Connection::State::DISCONNECTING;
	resetConnection();
	return expected;
}

template <typename _MsgType>
void network::Client<_MsgType>::disconnect()
{
	std::cout << "[Client] Manually disconnecting..." << std::endl;
	_connection.setState(Connection::State::DISCONNECTING);
	enet_peer_disconnect(
		_server,
		0		// data to send on disconnect, nothing for now
	);

	// no need to set disconnected state manually, disconnected event should be received in the read while loop.
	// If it is not, then it will still be updated after timeout anyway
}

template <typename _MsgType>
bool network::Client<_MsgType>::isConnected()
{
	return _connection.getState() == Connection::State::CONNECTED;
}

template <typename _MsgType>
network::Client<_MsgType>::InputQueue& network::Client<_MsgType>::getInQueue()
{
	return _readQueue;
}

template <typename _MsgType>
void network::Client<_MsgType>::onConnected(ENetEvent* event)
{
	std::cout << "[Client] Connected to server!" << std::endl;
	_connection.setState(Connection::State::CONNECTED);
}

template <typename _MsgType>
bool network::Client<_MsgType>::onDisconnected(ENetEvent* event)
{
	std::cout << "[Client] Disconnected from server" << std::endl;
	return true;
}

template <typename _MsgType>
ENetHost* network::Client<_MsgType>::getHost() const
{
	return _client.getHost();
}

template <typename _MsgType>
template <_MsgType type>
void network::Client<_MsgType>::send()
{
	if (isConnected())
		_msgService.send<type>(_server);
}

template <typename _MsgType>
template <_MsgType type, typename _MsgBody>
void network::Client<_MsgType>::send(const _MsgBody& body)
{
	if (isConnected())
		_msgService.send<type>(_server, body);
}

template <typename _MsgType>
void network::Client<_MsgType>::readEvents(int timeout, int interval)
{
	EventReader reader;
	auto lastUpdated = network::currentTime();
	while (true)
	{
		auto now = network::currentTime();
		auto timeSinceLastMsg = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdated);
		if (timeSinceLastMsg >= std::chrono::milliseconds(timeout))
		{
			std::cout << "[Client] Connection timed out!" << std::endl;
			return;
		}

		bool disconnected = reader.read(*this, _readQueue, interval, lastUpdated);
		if (disconnected)
			return;
	}
}

template <typename _MsgType>
void network::Client<_MsgType>::resetConnection()
{
	enet_peer_reset(_server);
	_connection.setState(Connection::State::DISCONNECTED);
	std::cout << "[Client] Connection reset" << std::endl;
}