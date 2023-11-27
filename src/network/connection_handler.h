#pragma once

#include <enet/enet.h>
#include <mutex>

#include "buffer.h" // TODO: remove this?
#include "messages.h" // TODO: remove this?

namespace network
{
	namespace Bandwidth {
		constexpr auto INF = 0;
	}

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

	enum ConnectionState {
		DISCONNECTED, DISCONNECTING, CONNECTING, CONNECTED
	};

	class ConnectionHandler 
	{
	public:
		ConnectionHandler(const ENetAddress& server);

	public:

		/*
		* Returns true if it terminated naturally due to disconnection, false if there was unexpected error
		*/
		bool connect(int timeout);

		void disconnect();

		// TODO: abstract this to other class
		template <messages::Type type>
		void send()
		{
			_writeBuffer.clear();
			_writeBuffer.write(type);

			auto data = _writeBuffer.data();
			auto size = _writeBuffer.size();
			ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);

			int result = -1;
			while (!result && getState() == ConnectionState::CONNECTED)
			{
				// it's possible for failure due to old or nullptr _server being used, in that case new _server will be fetched in next iteration and sent properly
				result = enet_peer_send(_server, 0, packet);		
			}
		}

		// TODO: abstract this to other class
		template <messages::Type type, typename T>
		void send(const T& body)
		{
			_writeBuffer.clear();
			_writeBuffer.write(body);
			_writeBuffer.write(type);

			auto data = _writeBuffer.data();
			auto size = _writeBuffer.size();
			ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);

			int result = -1;
			while (result < 0)
			{
				if (getState() != ConnectionState::CONNECTED) {
					std::cout << "[ConnectionHandler] Message didn't send because state was disconnected" << std::endl;
					return;
				}
				result = enet_peer_send(_server, 0, packet);		
			}
		}


	public:
		ConnectionState getState();
	private:
		void setState(ConnectionState);
		void readEvents(int timeout);

	private:
		Host _client;
		ENetPeer* _server;
		ENetAddress _serverAddress;
		ConnectionState _state;
		network::Buffer _writeBuffer;

		std::mutex _mux;
	};
}