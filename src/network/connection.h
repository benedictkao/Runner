#pragma once

#include <enet/enet.h>
#include "buffer.h"
#include "messages.h"

template <messages::Type type>
void sendMessage(ENetPeer* peer, network::Buffer& buffer)
{
	buffer.clear();
	buffer.write(type);

	auto data = buffer.data();
	auto size = buffer.size();
	ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

template <messages::Type type, typename T>
void sendMessage(ENetPeer* peer, network::Buffer& buffer, const T& body)
{
	buffer.clear();
	buffer.write(body);
	buffer.write(type);

	auto data = buffer.data();
	auto size = buffer.size();
	ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}