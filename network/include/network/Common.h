#pragma once

#include <enet/enet.h>

#include <chrono>
#include <mutex>

#include "Buffer.h"

namespace network
{
	typedef std::chrono::steady_clock clock;
	typedef clock::time_point TimeUnit;

	inline TimeUnit currentTime()
	{
		return clock::now();
	}

	namespace Bandwidth {
		constexpr int INF = 0;
	}

	/*
	* Wrapper class for ENet host for automatic cleanup on destroy
	*/
	class Host
	{
	public:
		Host(const char* hostName, int port, int maxConnections, int numChannels = 1, int inBandwidth = Bandwidth::INF, int outBandwidth = Bandwidth::INF);
		Host(int maxConnections, int numChannels = 1, int inBandwidth = Bandwidth::INF, int outBandwidth = Bandwidth::INF);
		~Host();

	public:
		ENetHost* getHost() const;

	private:
		ENetHost* _host;
	};

	/*
	* Helper class to encode messages as ENet packets
	*/
	class MessageService
	{
	public:
		int send(ENetPeer* peer, const Buffer& buffer)
		{
			ENetPacket* packet = createPacket(buffer);
			return enet_peer_send(peer, 0, packet);
		}

	private:
		/*
		* Note: this method is not thread-safe.
		* Caller is responsible for de-allocating packet memory
		*/
		ENetPacket* createPacket(const Buffer& buffer)
		{
			auto data = buffer.data();
			auto size = buffer.size();
			return enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE); // TODO: is this flag necessary?
		}
	};

	template <typename _MsgType>
	class BufferWriter
	{
	public:
		template <_MsgType type>
		const Buffer& writeToBuffer()
		{
			return _writeBuffer.write(type);
		}

		template <_MsgType type, typename _MsgBody>
		const Buffer& writeToBuffer(const _MsgBody& body)
		{
			return _writeBuffer
				.write(body)
				.write(type);
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