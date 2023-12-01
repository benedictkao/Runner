#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "demo_messages.h"
#include "common/network.h"

static constexpr int MAX_NUM_CLIENTS{ 32 };

struct ClientMetaData
{
	int id;
	network::TimeUnit lastPing;
};

void forward(ENetPeer* peer, const ENetEvent& event)
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
	network::Host server(&address, MAX_NUM_CLIENTS);

	std::mutex mux;
	std::unordered_map<ENetPeer*, ClientMetaData> clients(MAX_NUM_CLIENTS);
	std::queue<int> availableIds;
	for (int i = 0; i < MAX_NUM_CLIENTS; ++i)
	{
		availableIds.push(i);
	}

	// std::thread removeStaleClients([&]()
	// {
	// 	const auto interval = std::chrono::seconds(10);
	// 	network::Buffer writeBuffer(128);
	// 	while (true)
	// 	{
	// 		std::vector<ENetPeer*> staleClients;
	// 		network::TimeUnit now = network::currentTime();
	// 		mux.lock();
	// 		auto it = clients.begin();
	// 		while (it != clients.end())
	// 		{
	// 			auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastPing);
	// 			if (elapsedTime >= interval)
	// 			{
	// 				enet_peer_reset(it->first);
	// 				it = clients.erase(it);	// erasing will return the next key-value pair
	// 			}
	// 			else
	// 				++it;
	// 		}
	// 		mux.unlock();
	// 		std::this_thread::sleep_for(interval);
	// 	}
	// });

	// GAME LOOP START

	network::Buffer readBuffer(128);
	network::MessageService<messages::Type> msgService;
	while (true) 
	{
		while (ENetEvent* event = server.read(20))
		{
			switch (event->type)
			{
			case ENET_EVENT_TYPE_CONNECT: 
			{
				int id = availableIds.front();
				availableIds.pop();
				{
					//std::lock_guard lg(mux);
					clients[event->peer] = { id, network::currentTime() };
				}
				std::cout << "A new client connected from " << event->peer->address.host << ':' << event->peer->address.port << ", assigned id " << id << std::endl;
				msgService.send<messages::Type::LOGIN>(event->peer, id);
			}
			break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::cout << "Received packet from " << event->peer->address.host << ':' << event->peer->address.port << std::endl;

				messages::Type type;
				readBuffer.fill(event->packet->data, event->packet->dataLength);
				readBuffer.read(type);
				if (type == messages::Type::PING)
				{
					std::cout << "Returning pong to " << event->peer->address.host << ':' << event->peer->address.port << std::endl;
					forward(event->peer, *event);
				}
				else if (type == messages::Type::CHAT_MSG)
				{
					//std::lock_guard lg(mux);
					for (const auto pair : clients) {
						if (pair.first == event->peer) {
							continue;
						}
						std::cout << "Sending data to " << pair.second.id << std::endl;
						forward(pair.first, *event);
					}
				}
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				{
					//std::lock_guard lg(mux);
					clients.erase(event->peer);
				}
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

	// removeStaleClients.join();
	return EXIT_SUCCESS;
}