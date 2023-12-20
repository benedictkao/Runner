#pragma once

#include <iostream>
#include <mutex>
#include <unordered_set>
#include <vector>

#include "Common.h"
#include "ConcurrentSet.h"
#include "ConcurrentMap.h"
#include "EventReader.h"

namespace network
{
	template <typename _ConnectionCallback = EmptyConnectionCallback>
	class Server
	{
	public:
		Server(int port, int maxConnections);
		Server(int port, int maxConnections, _ConnectionCallback& callback);

	/*
	* Interface methods for EventReader::read template arg _Connection
	*/
	public:
		void onConnected(ENetEvent*);
		bool onDisconnected(ENetEvent*);
		void onReceive(ENetEvent*);
		ENetHost* getHost() const;
		ConcurrentQueue<InMessage>& getInQueue();

	public:
		void run(int interval);
		bool isConnected(ENetPeer* client);
		std::vector<ENetPeer*> getClients();
		std::vector<ENetPeer*> getActiveClients(int expiryTime);

	private:
		inline Host createHost(int port, int maxConnections) const;

	private:
		Host _host;
		ConcurrentMap<ENetPeer*, TimeUnit> _clients;
		ConcurrentQueue<InMessage> _readQueue;
		_ConnectionCallback* _callback;
	};
}

#include "Server.tpp"