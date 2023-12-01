#pragma once

#include <enet/enet.h>
#include "mux_queue.h"
#include "network.h"

namespace network
{
	namespace ClientConst
	{
		constexpr int DEFAULT_READ_TIMEOUT { 30000 };
		constexpr int DEFAULT_READ_INTERVAL { 20 };
	}

	template <typename T>
	class Client
	{
	public:
		typedef mux_queue<network::Buffer> InputQueue;

	public:
	Client(const ENetAddress& server);

	public:
		/*
		* Returns true if it terminated naturally due to manual disconnection or timeout, false if there was unexpected error
		*/
		bool connect(int timeout = ClientConst::DEFAULT_READ_TIMEOUT, int interval = ClientConst::DEFAULT_READ_INTERVAL);
		void disconnect();
		bool isConnected();
		InputQueue& getInQueue();

	public:
		template <T type>
		void send();

		template <T type, typename V>
		void send(const V& body);

	private:
		void readEvents(int timeout, int interval);
		void resetConnection();

	private:
		Host _client;
		ENetPeer* _server;
		ENetAddress _serverAddress;
		Connection _connection;
		MessageService<T> _msgService;
		InputQueue _readQueue;
	};
}

#include "Client.tpp"