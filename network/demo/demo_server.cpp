#include <iostream>
#include <thread>

#include "demo_messages.h"

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

	network::Server server(PORT_NUMBER, MAX_NUM_CLIENTS);
	std::thread readThread([&]() {
		server.run(20);
	});

	// GAME LOOP START
	
	auto& inQueue = server.getInQueue();
	auto& clients = server.getClients();
	network::BufferWriter<messages::Type> writer;
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
				auto& buffer = writer.writeToBuffer<messages::Type::PING>();
				network::MessageService::send(inMsg.source, buffer);
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
					auto& buffer = writer.writeToBuffer<messages::Type::CHAT_MSG>(msg);
					network::MessageService::send(client, buffer);
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