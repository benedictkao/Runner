#include <iostream>

#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "message_queue.h"
#include "connection.h"

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

	// create an enet server
	ENetHost* server;
	ENetAddress address;
	address.host = ENET_HOST_ANY;	// bind or connect to any available IP address
	address.port = 7777;
	server = enet_host_create(
		&address,	// the address to bind the server host to
		32,			// number of outgoing connections, server can connect to up to 32 clients
		1,			// number of channels, 1 for now
		0,			// incoming bandwidth, 0 means infinite
		0			// outgoing bandwidth, 0 means infinite
	);

	if (server == NULL)
	{
		std::cout << "Error occurred while trying to create an ENet server host." << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::cout << "ENet host created successfully" << std::endl;
	}

	std::unordered_set<ENetPeer*> clients;
	message_queue inQueue;
	network::Buffer writeBuffer;

	// GAME LOOP START

	while (true) 
	{
		ENetEvent event;
		while (enet_host_service(server, &event, 50) > 0) // check data 50ms delay
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT: 
			{
				auto id = event.peer;
				clients.insert(id);
				std::cout << "A new client connected from " << event.peer->address.host << ':' << event.peer->address.port << ", assigned id " << id << std::endl;
				sendMessage<messages::Type::LOGIN>(event.peer, writeBuffer, id);
			}
			break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::cout << "Received packet from " << event.peer->address.host << ':' << event.peer->address.port << std::endl;
				for (auto client : clients) {
					if (client == event.peer) {
						continue;
					}
					std::cout << "Sending data to " << client << std::endl;
					forward(client, event);
				}
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				clients.erase(event.peer);
				std::cout << event.peer->address.host << ':' << event.peer->address.port << " disconnected." << std::endl;
				event.peer->data = NULL;	// clean up the data
			}
			break;
			default:
			break;
			}
		}
	}

	// GAME LOOP END

	enet_host_destroy(server);

	return EXIT_SUCCESS;
}