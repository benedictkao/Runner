#include <network/Client.h>

#include <iostream>
#include <atomic>

network::Client::Client(const ENetAddress& server): _client(NULL, 1), _server(nullptr), _serverAddress(server) {}

bool network::Client::connect(int timeout, int interval)
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

void network::Client::disconnect()
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

bool network::Client::isConnected()
{
	return _connection.getState() == Connection::State::CONNECTED;
}

network::Client::InputQueue& network::Client::getInQueue()
{
	return _readQueue;
}

void network::Client::onConnected(ENetEvent* event)
{
	std::cout << "[Client] Connected to server!" << std::endl;
	_connection.setState(Connection::State::CONNECTED);
}

bool network::Client::onDisconnected(ENetEvent* event)
{
	std::cout << "[Client] Disconnected from server" << std::endl;
	return true;
}

ENetHost* network::Client::getHost() const
{
	return _client.getHost();
}

void network::Client::send(const Buffer& buffer)
{
	if (isConnected())
		_msgService.send(_server, buffer);
}

void network::Client::readEvents(int timeout, int interval)
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

void network::Client::resetConnection()
{
	enet_peer_reset(_server);
	_connection.setState(Connection::State::DISCONNECTED);
	std::cout << "[Client] Connection reset" << std::endl;
}