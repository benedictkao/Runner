#pragma once

#include <enet/enet.h>
#include "EventReader.h"
#include "ConcurrentQueue.h"
#include "network.h"

namespace network
{
	namespace ClientConst
	{
		constexpr int DEFAULT_READ_TIMEOUT { 30000 };
		constexpr int DEFAULT_READ_INTERVAL { 20 };
	}

	template <typename _MsgType>
	class Client
	{
	public:
		typedef ConcurrentQueue<InMessage> InputQueue;

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

	/*
	* Interface methods for EventReader::read template arg _Connection
	*/
	public:
		void onConnected(ENetEvent*);
		bool onDisconnected(ENetEvent*);
		ENetHost* getHost() const;

	public:
		template <_MsgType type>
		void send();

		template <_MsgType type, typename _MsgBody>
		void send(const _MsgBody& body);

	private:
		void readEvents(int timeout, int interval);
		void resetConnection();

	private:
		Host _client;
		ENetPeer* _server;
		ENetAddress _serverAddress;
		Connection _connection;
		MessageService<_MsgType> _msgService;
		InputQueue _readQueue;
	};
}

#include "Client.tpp"