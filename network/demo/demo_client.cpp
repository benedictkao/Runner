#include <iostream>

#include <thread>
#include <string>

#include "network/Client.h"
#include "demo_messages.h"

static constexpr auto HOST_NAME{ "127.0.0.1" };
static constexpr auto TIMEOUT{ 10000 };
static constexpr auto PORT{ 7777 };

int id = -1;
bool quit = false;
network::TimeUnit lastPing;

void stayConnected(network::Client& client) 
{
	while (!quit)
	{
		bool expectedDisconnect = client.connect(TIMEOUT);
		if (expectedDisconnect)
		{
			std::cout << "[main] Connection ended" << std::endl;
			return;
		}
		else
		{
			// unnatural disconnection, wait awhile before trying again
			std::cout << "[main] Retrying connection in 5s" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		}
	}
}

void sendChatMsg(network::Client& client)
{
	std::string in;
	network::BufferWriter<messages::Type> writer;
	while (!quit)
	{
		std::getline(std::cin, in);
		messages::body::ChatMsg message;
		message.id = id;	// this is not thread-safe so might not get most updated value, but  it's ok
		std::strcpy(message.message, in.c_str());
		if (in.empty())
		{
			quit = true;
			client.disconnect();
		}
		else
		{	
			auto& buffer = writer.writeToBuffer<messages::Type::CHAT_MSG>(message);
			client.send(buffer);
		}
	}
}

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
		std::cout << "[main] Successful client ENet init!" << std::endl;
	}
	atexit(enet_deinitialize);
	
	ENetAddress address;

	// set host to connect to
	enet_address_set_host(&address, HOST_NAME);
	address.port = PORT;
	network::Client client(address);

	std::thread read_thread(stayConnected, std::ref(client));
	std::thread ping_thread([&]() {
		network::BufferWriter<messages::Type> writer;
		while (!quit)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
			if (client.isConnected())
			{
				auto& buffer = writer.writeToBuffer<messages::Type::PING>();
				client.send(buffer);
				auto now = network::currentTime();
				lastPing = now;
			}
		}
	});
	std::thread write_thread(sendChatMsg, std::ref(client));

	// GAME LOOP START
	
	auto& inQueue = client.getInQueue();
	while (!quit)
	{
		auto numMsgs = inQueue.size();
		for (int i = 0; i < numMsgs; ++i)
		{
			network::Buffer buffer = inQueue.popFront().raw;
			messages::Type type;
			buffer.read(type);
			switch (type) {
			case messages::Type::PING:
			{
				auto pong = network::currentTime();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(pong - lastPing).count();
				std::cout << "[main] Server ping: " << duration << "ms" << std::endl;
			}
			break;
			case messages::Type::LOGIN:
			{
				messages::body::Login loginBody;
				buffer.read(loginBody);
				id = loginBody.id;
				std::cout << "[main] You are connected to the server! Your id is " << id << "\n++++++++++++++" << std::endl;
			}
			break;
			case messages::Type::CHAT_MSG:
			{
				messages::body::ChatMsg msg;
				buffer.read(msg);
				std::cout << msg.id << ": " << msg.message << std::endl;
			}
			break;
			default:
			{
				auto data = buffer.data();
				std::cout << "[main] Unknown message received: " << data;
			}
			break;
			}
		}
		// manually add 20ms delay to reduce cpu usage, similar sleep behaviour is present for games with controlled frame rates
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	// GAME LOOP END

	read_thread.join();
	std::cout << "read thread join" << std::endl;
	ping_thread.join();
	std::cout << "ping thread join" << std::endl;
	write_thread.join();
	std::cout << "write thread join" << std::endl;

	return EXIT_SUCCESS;
}