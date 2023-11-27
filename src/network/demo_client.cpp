#include <iostream>

#include <thread>
#include <string>
#include <string_view>
#include <mutex>
#include <future>
#include <condition_variable>
#include "connection.h"
#include "connection_handler.h"

static constexpr auto HOST_NAME{ "127.0.0.1" };
static constexpr auto TIMEOUT{ 10000 };
static constexpr auto PORT{ 7777 };

void stayConnected(network::ConnectionHandler& connection) 
{
	while (true)
	{
		bool expectedDisconnect = connection.connect(TIMEOUT);
		if (!expectedDisconnect)
			// unnatural disconnection, wait awhile before trying again
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
}

void readNetworkMsgs(ENetHost* client, std::promise<bool>& loginResult) 
{
	network::Buffer readBuffer;
	ENetEvent event;
	while (true) 
	{
		while (enet_host_service(client, &event, 30000) > 0) // check data with 30s timeout
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE: 
			{
				readBuffer.fill(event.packet->data, event.packet->dataLength);
				messages::Type type;
				readBuffer.read<messages::Type>(type);
				switch (type) {
				case messages::Type::LOGIN:
					/*if (!id) {
						readBuffer.read<ENetPeer*>(id);
						loginResult.set_value(true);
						std::cout << "You are connected to the server! Your id is " << id << "\n++++++++++++++" << std::endl;
					}*/
					break;
				case messages::Type::CHAT_MSG:
				{
					messages::body::ChatMsg msg;
					readBuffer.read<messages::body::ChatMsg>(msg);
					std::cout << msg.sender << ": " << msg.message << std::endl;
				}
				break;
				default:
					auto data = readBuffer.data();
					std::cout << "Unknown message received: " << data;
				}
				break;
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << "Disconnection succeeded." << std::endl;
				//loginResult.set_value(false);
				// TODO: break out of outer block and restart the read function with new promise
				break;
			default:
				break;
			}
		}
	}
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
		std::cout << "Successful client ENet init!" << std::endl;
	}
	atexit(enet_deinitialize);
	
	ENetAddress address;

	// set host to connect to
	enet_address_set_host(&address, HOST_NAME);
	address.port = PORT;
	network::ConnectionHandler connection(address);

	std::thread read_thread(stayConnected, std::ref(connection));
	std::string in;

	// GAME LOOP START

	//while (connection.getState() == network::ConnectionState::CONNECTED) {
	while (true)
	{
		std::getline(std::cin, in);
		messages::body::ChatMsg message;
		message.sender = nullptr;
		std::strcpy(message.message, in.c_str());
		connection.send<messages::Type::CHAT_MSG, messages::body::ChatMsg>(message);
	}

	// GAME LOOP END

	read_thread.join();

	connection.disconnect();

	return EXIT_SUCCESS;
}