#include "connection_handler.h"

#include <iostream>
#include <utility>

network::Host::Host(ENetAddress* address, int maxConnections, int numChannels, int inBandwidth, int outBandwidth)
{
	_host = enet_host_create(
		address,									// the address to bind the server host to
		static_cast<size_t>(maxConnections),			// number of outgoing connections, server can connect to up to 32 clients
		static_cast<size_t>(numChannels),				// number of channels, 1 for now
		static_cast<enet_uint32>(inBandwidth),			// incoming bandwidth, 0 means infinite
		static_cast<enet_uint32>(outBandwidth)			// outgoing bandwidth, 0 means infinite
	);
	if (_host == NULL) {
		std::cout << "[Host] Error occurred while trying to create an ENet host." << std::endl;
		// TODO: handle host not created error
	}
	else {
		std::cout << "[Host] ENet host created successfully" << std::endl;
	}
}

network::Host::~Host()
{
	enet_host_destroy(_host);
}

ENetEvent* network::Host::read(int timeout)
{
	return enet_host_service(_host, &_event, timeout) > 0 ? &_event : nullptr;
}

ENetHost * network::Host::getHost() const
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

network::ConnectionManager::ConnectionManager(const ENetAddress& server): _client(NULL, 1), _server(nullptr), _serverAddress(server) {}

bool network::ConnectionManager::connect(int timeout)
{
	if (_connection.getState() != Connection::State::DISCONNECTED)
		return false;
	
	_connection.setState(Connection::State::CONNECTING);
	std::cout << "[ConnectionHandler] Attempting new connection to " << _serverAddress.host << ':' << _serverAddress.port << "..." << std::endl;

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
		std::cout << "[ConnectionHandler] Connection failed, no available peers for initiating an ENet connection." << std::endl;
		_connection.setState(Connection::State::DISCONNECTED);
		return false;
	}

	// this is a blocking call, and should not return unless connection is broken
	readEvents(timeout);

	// no data received within timeout interval, treat it as if it's disconnected
	std::cout << "[ConnectionHandler] Connection timed out!" << std::endl;
	disconnect();
	
	std::cout << "[ConnectionHandler] Disconnected from server" << std::endl;
	_connection.setState(Connection::State::DISCONNECTED);
	return true;
}

void network::ConnectionManager::disconnect()
{
	std::cout << "[ConnectionHandler] Manually disconnecting..." << std::endl;
	_connection.setState(Connection::State::DISCONNECTING);
	enet_peer_disconnect(
		_server,
		0		// data to send on disconnect, nothing for now
	);

	// no need to set disconnected state manually, disconnected event should be received in the read while loop.
	// If it is not, then it will still be updated after timeout anyway
}

mux_queue<network::Buffer>& network::ConnectionManager::getInQueue()
{
	return _readQueue;
}

void network::ConnectionManager::readEvents(int timeout)
{
	while (ENetEvent* event = _client.read(timeout))
	{
		switch (event->type) 
		{
			case ENET_EVENT_TYPE_CONNECT:
				std::cout << "[ConnectionHandler] Connected to server!" << std::endl;
				_connection.setState(Connection::State::CONNECTED);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				if (!isConnected()) 
				{
					enet_packet_destroy(event->packet);
					break;
				}
				network::Buffer readBuffer;
				readBuffer.fill(event->packet->data, event->packet->dataLength);
				_readQueue.push(std::move(readBuffer));
			}
				break;
			case ENET_EVENT_TYPE_DISCONNECT: 
			{
				std::cout << "[ConnectionHandler] Disconnected from server" << std::endl;
				_connection.setState(Connection::State::DISCONNECTED);
				return;
			}
			default:
				break;
		}
	}
}

bool network::ConnectionManager::isConnected()
{
	return _connection.getState() == Connection::State::CONNECTED;
}