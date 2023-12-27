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

	template <typename _Duration>
	inline long long getMillis(_Duration duration)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	template <typename _Duration>
	inline long long getMicros(_Duration duration)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	}

	inline bool compareMillis(const TimeUnit& a, const TimeUnit& b, int threshold)
	{
		return getMillis(a - b) > threshold;
	}

	namespace Bandwidth {
		constexpr int INF = 0;
	}

	int initENet();

	/*
	* Wrapper class for ENet host for automatic cleanup on destroy
	*/
	class Host
	{
	public:
		Host(ENetAddress* address, int maxConnections, int numChannels = 1, int inBandwidth = Bandwidth::INF, int outBandwidth = Bandwidth::INF);
		~Host();

	public:
		ENetHost* getHost() const;

	private:
		ENetHost* _host;
	};

	/*
	* Helper class to encode messages as ENet packets
	*/
	namespace MessageService
	{
		int send(ENetPeer* peer, const Buffer& buffer, enet_uint32 flags = ENET_PACKET_FLAG_UNSEQUENCED);

		/*
		* Note: this method is not thread-safe.
		* Caller is responsible for de-allocating packet memory
		*/
		ENetPacket* createPacket(const Buffer& buffer, enet_uint32 flags);
	};

	template <typename _MsgType>
	class BufferWriter
	{
	public:
		template <_MsgType type>
		const Buffer& writeToBuffer()
		{
			_writeBuffer.clear();
			return _writeBuffer.write(type);
		}

		template <_MsgType type, typename _MsgBody>
		const Buffer& writeToBuffer(const _MsgBody& body)
		{
			_writeBuffer.clear();
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
		State getRecentState() const;
		void setState(State);

	private:
		State _state;
		std::mutex _mux;
	};

	struct EmptyConnectionCallback {};
}