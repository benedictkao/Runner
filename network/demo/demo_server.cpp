#include <iostream>
#include <thread>

#include "demo_messages.h"
#include "network/network.h"
#include "network/Server.h"

static constexpr int MAX_NUM_CLIENTS{ 32 };
static constexpr int PORT_NUMBER{ 7777 };

int main(int argc, char* argv[])
{
	// initialise enet
	if (enet_initialize() != 0)
	{
		std::cout << "[main] An error occurred during initializing ENet." << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::cout << "[main] Successful server ENet init!" << std::endl;
	}
	atexit(enet_deinitialize);

	ENetAddress address;
	address.host = ENET_HOST_ANY;	// bind or connect to any available IP address
	address.port = PORT_NUMBER;
	network::Server server(address, MAX_NUM_CLIENTS);
	std::thread readThread([&]() {
		server.run(20);
	});

	network::MessageService<messages::Type> msgService;

	// GAME LOOP START
	
	auto& inQueue = server.getInQueue();
	auto& clients = server.getClients();
	while (true)
	{
		auto numMsgs = inQueue.size();
		for (int i = 0; i < numMsgs; ++i)
		{
			auto inMsg = inQueue.popFront();
			messages::Type type;
			inMsg.raw.read(type);
			switch (type) {
			case messages::Type::PING:
			{
				std::cout << "[main] Ping received from " << inMsg.source << ", returning pong" << std::endl;
				msgService.send<messages::Type::PING>(inMsg.source);
			}
			break;
			case messages::Type::CHAT_MSG:
			{
				std::cout << "[main] Chat message received from " << inMsg.source << ", forwarding to other clients" << std::endl;
				auto clientList = clients.toList();
				messages::body::ChatMsg msg;
				inMsg.raw.read(msg);
				for (auto client : clientList)
				{
					if (client == inMsg.source)
						continue;
					msgService.send<messages::Type::CHAT_MSG>(client, msg);
				}
			}
			break;
			default:
			{
				auto data = inMsg.raw.data();
				std::cout << "[main] Unknown message received: " << data << std::endl;
			}
			break;
			}
		}
	}

	// GAME LOOP END

	readThread.join();

	return EXIT_SUCCESS;
}