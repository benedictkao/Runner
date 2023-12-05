#pragma once

#include "Common.h"
#include "EventReader.h"
#include "ConcurrentQueue.h"

namespace network
{
	namespace ClientConst
	{
		constexpr int DEFAULT_READ_TIMEOUT { 30000 };
		constexpr int DEFAULT_READ_INTERVAL { 20 };
	}

	class Client
	{
	public:
		typedef ConcurrentQueue<InMessage> InputQueue;

	public:
		Client(const char* serverName, int port);

	public:
		/*
		* Returns true if it terminated naturally due to manual disconnection or timeout, false if there was unexpected error
		*/
		bool connect(int timeout = ClientConst::DEFAULT_READ_TIMEOUT, int interval = ClientConst::DEFAULT_READ_INTERVAL);
		void disconnect();
		bool isConnected();
		InputQueue& getInQueue();

	/*
	* Interface methods for EventReader::read template arg _Connection
	*/
	public:
		void onConnected(ENetEvent*);
		bool onDisconnected(ENetEvent*);
		ENetHost* getHost() const;

	public:
		void send(const Buffer&);

	private:
		void readEvents(int timeout, int interval);
		void resetConnection();

	private:
		Host _client;
		ENetPeer* _server;
		ENetAddress _serverAddress;
		Connection _connection;
		MessageService _msgService;
		InputQueue _readQueue;
	};
}