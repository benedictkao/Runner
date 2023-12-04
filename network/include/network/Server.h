#pragma once

#include <iostream>
#include <mutex>
#include <unordered_set>
#include <vector>

#include <enet/enet.h>

#include "ConcurrentSet.h"
#include "EventReader.h"
#include "network.h"

namespace network
{
	class Server
	{
	public:
		Server(ENetAddress& address, int maxConnections);

	/*
	* Interface methods for EventReader::read template arg _Connection
	*/
	public:
		void onConnected(ENetEvent*);
		bool onDisconnected(ENetEvent*);
		ENetHost* getHost() const;
		ConcurrentQueue<InMessage>& getInQueue();

	public:
		void run(int interval);
		bool isConnected(ENetPeer* client);
		ConcurrentSet<ENetPeer*>& getClients();

	private:
		Host _host;
		ConcurrentSet<ENetPeer*> _clients;
		ConcurrentQueue<InMessage> _readQueue;
	};
}

network::Server::Server(ENetAddress& address, int maxConnections): _host(&address, maxConnections), _clients(maxConnections) {}

void network::Server::onConnected(ENetEvent* event)
{
	std::cout << "[Server] A new client connected from " << event->peer->address.host << ':' << event->peer->address.port << std::endl;
	_clients.insert(event->peer);
}

bool network::Server::onDisconnected(ENetEvent* event)
{
	std::cout << "[Server] Client " << event->peer->address.host << ':' << event->peer->address.port << " disconnected." << std::endl;
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