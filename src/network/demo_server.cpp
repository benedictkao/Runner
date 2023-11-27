#include <iostream>

#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "message_queue.h"
#include "connection.h"
#include "connection_handler.h"

void forward(ENetPeer* peer, ENetEvent event)
{
	const void* data = event.packet->data;
	size_t size = event.packet->dataLength;
	ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);	// will retry if fail
	enet_peer_send(peer, 0, packet);	// sends on channel 0
}

int main(int argc, char* argv[])
{
	// initialise enet
	if (enet_initialize() != 0)
	{
		std::cout << "An error occurred during initializing ENet." << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::cout << "Successful server ENet init!" << std::endl;
	}
	atexit(enet_deinitialize);

	ENetAddress address;
	address.host = ENET_HOST_ANY;	// bind or connect to any available IP address
	address.port = 7777;
	network::Host server(&address, 32);

	std::unordered_set<ENetPeer*> clients;
	message_queue inQueue;
	network::Buffer writeBuffer;

	// GAME LOOP START

	while (true) 
	{
		while (ENetEvent* event = server.read(50))
		{
			switch (event->type)
			{
			case ENET_EVENT_TYPE_CONNECT: 
			{
				auto id = event->peer;
				clients.insert(id);
				std::cout << "A new client connected from " << event->peer->address.host << ':' << event->peer->address.port << ", assigned id " << id << std::endl;
				sendMessage<messages::Type::LOGIN>(event->peer, writeBuffer, id);
			}
			break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::cout << "Received packet from " << event->peer->address.host << ':' << event->peer->address.port << std::endl;
				for (auto client : clients) {
					if (client == event->peer) {
						continue;
					}
					std::cout << "Sending data to " << client << std::endl;
					forward(client, *event);
				}
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				clients.erase(event->peer);
				std::cout << event->peer->address.host << ':' << event->peer->address.port << " disconnected." << std::endl;
				event->peer->data = NULL;	// clean up the data
			}
			break;
			default:
			break;
			}
		}
	}

	// GAME LOOP END

	return EXIT_SUCCESS;
}