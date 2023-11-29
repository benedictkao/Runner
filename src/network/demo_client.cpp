#include <iostream>

#include <thread>
#include <string>
#include <string_view>
#include <mutex>
#include <future>

#include "connection.h"
#include "connection_handler.h"
#include "messages.h"

static constexpr auto HOST_NAME{ "127.0.0.1" };
static constexpr auto TIMEOUT{ 10000 };
static constexpr auto PORT{ 7777 };

int id = -1;
network::TimeUnit lastPing;

void stayConnected(network::ConnectionManager& connection) 
{
	while (true)
	{
		bool expectedDisconnect = connection.connect(TIMEOUT);
		if (!expectedDisconnect)
			// unnatural disconnection, wait awhile before trying again
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
}

void sendChatMsg(network::ConnectionManager& connection)
{
	std::string in;
	while (true)
	{
		std::getline(std::cin, in);
		messages::body::ChatMsg message;
		message.id = id;	// this is not thread-safe so might not get most updated value, but  it's ok
		std::strcpy(message.message, in.c_str());
		connection.send<messages::Type::CHAT_MSG, messages::body::ChatMsg>(message);
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
	network::ConnectionManager connection(address);

	std::thread read_thread(stayConnected, std::ref(connection));
	std::thread ping_thread([&]() {
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
			if (connection.isConnected())
			{
				connection.send<messages::Type::PING>();
				auto now = network::currentTime();
				lastPing = now;
			}
		}
	});
	std::thread write_thread(sendChatMsg, std::ref(connection));

	// GAME LOOP START

	while (true)
	{
		auto& inQueue = connection.getInQueue();
		auto numMsgs = inQueue.size();
		for (int i = 0; i < numMsgs; ++i)
		{
			network::Buffer buffer = inQueue.popFront();
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
				buffer.read<messages::body::Login>(loginBody);
				id = loginBody.id;
				std::cout << "[main] You are connected to the server! Your id is " << id << "\n++++++++++++++" << std::endl;
			}
			break;
			case messages::Type::CHAT_MSG:
			{
				messages::body::ChatMsg msg;
				buffer.read<messages::body::ChatMsg>(msg);
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
	ping_thread.join();
	write_thread.join();

	connection.disconnect();

	return EXIT_SUCCESS;
}