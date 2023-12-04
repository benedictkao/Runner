#include "network/Client.h"

#include "logging/Logger.h"

network::Client::Client(const char* serverName, int port): _client(1), _server(nullptr)
{
	enet_address_set_host(&_serverAddress, serverName);
	_serverAddress.port = port;
}

bool network::Client::connect(int timeout, int interval)
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

void network::Client::disconnect()
{
	debug::log("[Client] Manually disconnecting...");
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
	debug::log("[Client] Connected to server!");
	_connection.setState(Connection::State::CONNECTED);
}

bool network::Client::onDisconnected(ENetEvent* event)
{
	debug::log("[Client] Disonnected to server");
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
			debug::log("[Client] Connection timed out!");
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
	debug::log("[Client] Connection reset");
}