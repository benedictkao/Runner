template <typename T>
network::Client<T>::Client(const ENetAddress& server): _client(NULL, 1), _server(nullptr), _serverAddress(server) {}

template <typename T>
bool network::Client<T>::connect(int timeout, int interval)
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

template <typename T>
void network::Client<T>::disconnect()
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

template <typename T>
bool network::Client<T>::isConnected()
{
	return _connection.getState() == Connection::State::CONNECTED;
}

template <typename T>
template <T type>
void network::Client<T>::send()
{
	_msgService.send<type>(_server);
}

template <typename T>
template <T type, typename V>
void network::Client<T>::send(const V& body)
{
	_msgService.send<type>(_server, body);
}

template <typename T>
mux_queue<network::Buffer>& network::Client<T>::getInQueue()
{
	return _readQueue;
}

template <typename T>
void network::Client<T>::readEvents(int timeout, int interval)
{
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

		while (ENetEvent* event = _client.read(interval))	// read with up to `interval` ms delay
		{
			switch (event->type) 
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "[Client] Connected to server!" << std::endl;
					_connection.setState(Connection::State::CONNECTED);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
				{
					// if (!isConnected()) 
					// {
					// 	enet_packet_destroy(event->packet);
					// 	return;
					// }
					lastUpdated = network::currentTime();
					network::Buffer readBuffer;
					readBuffer.fill(event->packet->data, event->packet->dataLength);
					_readQueue.push(std::move(readBuffer));
				}
					break;
				case ENET_EVENT_TYPE_DISCONNECT: 
				{
					std::cout << "[Client] Disconnected from server" << std::endl;
					return;
				}
				default:
					break;
			}
		}
	}
}

template <typename T>
void network::Client<T>::resetConnection()
{
	enet_peer_reset(_server);
	_connection.setState(Connection::State::DISCONNECTED);
	std::cout << "[Client] Connection reset" << std::endl;
}