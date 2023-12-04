#pragma once

#include <iostream>
#include <mutex>
#include <unordered_set>
#include <vector>

#include <enet/enet.h>

#include "ConcurrentSet.h"
#include "EventReader.h"
#include "Common.h"

namespace network
{
	class Server
	{
	public:
		Server(int port, int maxConnections);

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
		void send(ENetPeer* client, const Buffer&);

	private:
		Host _host;
		ConcurrentSet<ENetPeer*> _clients;
		ConcurrentQueue<InMessage> _readQueue;
		MessageService _msgService;
	};
}