#pragma once

#include <enet/enet.h>

#include <chrono>
#include <iostream>
#include <mutex>

#include "buffer.h"
#include "messages.h"
#include "mux_queue.h"

namespace network
{
	namespace Bandwidth {
		constexpr auto INF = 0;
	}

	typedef std::chrono::steady_clock::time_point TimeUnit;

	inline TimeUnit currentTime()
	{
		return std::chrono::steady_clock::now();
	}

	/*
	* Wrapper class for ENet host for automatic cleanup on destroy
	*/
	class Host
	{
	public:
		Host(ENetAddress* address, int maxConnections, int numChannels = 1, int inBandwidth = Bandwidth::INF, int outBandwidth = Bandwidth::INF);
		~Host();

	public:
		ENetEvent* read(int timeout);
		ENetHost* getHost() const;

	private:
		ENetHost* _host;
		ENetEvent _event;
	};

	/*
	* Class to track connection state in a mutually exclusive manner
	*/
	class Connection
	{
	public:
		enum State
		{
			DISCONNECTED, DISCONNECTING, CONNECTING, CONNECTED
		};

	public:
		Connection();

	public:
		State getState();
		void setState(State);

	private:
		State _state;
		std::mutex _mux;
	};

	class DataParser
	{
	public:
		/*
		* Note: this method is not thread-safe.
		* Class that calls this function is responsible for de-allocating packet memory
		*/
		template <messages::Type type>
		ENetPacket* createPacket()
		{
			_writeBuffer.write(type);
			auto data = _writeBuffer.data();
			auto size = _writeBuffer.size();
			ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
			_writeBuffer.clear();
			return packet;
		}

		/*
		* Note: this method is not thread-safe.
		* Class that calls this function is responsible for de-allocating packet memory
		*/
		template <messages::Type type, typename T>
		ENetPacket* createPacket(const T& body)
		{
			_writeBuffer.write(body);
			return createPacket<type>();
		}

	private:
		network::Buffer _writeBuffer;
	};

	class ConnectionManager 
	{
	public:
		typedef mux_queue<network::Buffer> InputQueue;

	public:
		ConnectionManager(const ENetAddress& server);

	public:
		/*
		* Returns true if it terminated naturally due to manual disconnection or timeout, false if there was unexpected error
		*/
		bool connect(int timeout);
		void disconnect();
		bool isConnected();
		InputQueue& getInQueue();

	public:
		// TODO: abstract this to other class
		template <messages::Type type>
		void send()
		{
			ENetPacket* packet = _parser.createPacket<type>();

			int result = -1;
			while (result < 0)
			{
				// it's possible for failure due to old or nullptr _server being read, in that case new _server will be fetched in next iteration and sent properly
				if (!isConnected()) {
					std::cout << "[ConnectionHandler] Message didn't send because state was disconnected" << std::endl;
					enet_packet_destroy(packet);
					return;
				}
				result = enet_peer_send(_server, 0, packet);		
			}
		}

		// TODO: abstract this to other class
		template <messages::Type type, typename T>
		void send(const T& body)
		{
			ENetPacket* packet = _parser.createPacket<type, T>(body);

			int result = -1;
			while (result < 0)
			{
				// it's possible for failure due to old or nullptr _server being read, in that case new _server will be fetched in next iteration and sent properly
				if (!isConnected()) {
					std::cout << "[ConnectionHandler] Message didn't send because state was disconnected" << std::endl;
					enet_packet_destroy(packet);
					return;
				}
				result = enet_peer_send(_server, 0, packet);		
			}
		}

	private:
		void readEvents(int timeout);

	private:
		Host _client;
		ENetPeer* _server;
		ENetAddress _serverAddress;
		Connection _connection;
		DataParser _parser;
		InputQueue _readQueue;
	};
}