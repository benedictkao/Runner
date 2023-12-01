#pragma once

#include <enet/enet.h>

#include <chrono>
#include <iostream>
#include <mutex>

#include "buffer.h"

namespace network
{
	namespace Bandwidth {
		constexpr auto INF = 0;
	}

	typedef std::chrono::steady_clock clock;
	typedef clock::time_point TimeUnit;

	inline TimeUnit currentTime()
	{
		return clock::now();
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
	* Helper class to encode messages as ENet packets
	*/
	template <typename T>
	class MessageService
	{
	public:
		template <T type>
		int send(ENetPeer* peer)
		{
			ENetPacket* packet = createPacket<type>();
			return enet_peer_send(peer, 0, packet);
		}

		template <T type, typename V>
		int send(ENetPeer* peer, const V& body)
		{
			ENetPacket* packet = createPacket<type>(body);
			return enet_peer_send(peer, 0, packet);
		}

	private:
		/*
		* Note: this method is not thread-safe.
		* Caller is responsible for de-allocating packet memory
		*/
		template <T type>
		ENetPacket* createPacket()
		{
			_writeBuffer.write(type);
			auto data = _writeBuffer.data();
			auto size = _writeBuffer.size();
			ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE); // TODO: is this flag necessary?
			_writeBuffer.clear();
			return packet;
		}

		/*
		* Note: this method is not thread-safe.
		* Caller is responsible for de-allocating packet memory
		*/
		template <T type, typename U>
		ENetPacket* createPacket(const U& body)
		{
			_writeBuffer.write(body);
			return createPacket<type>();
		}

	private:
		network::Buffer _writeBuffer;
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
}