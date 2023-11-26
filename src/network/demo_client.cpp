#include <iostream>

#include <thread>
#include <string>
#include <string_view>
#include <mutex>
#include <future>
#include <condition_variable>
#include "connection.h"

static constexpr auto HOST_NAME{ "127.0.0.1" };
static constexpr auto TIMEOUT{ 30000 };
static constexpr auto PORT{ 7777 };

ENetPeer* id = nullptr;

void connect(ENetHost* client) {
	ENetEvent event;
	while (true) {
		// wait for up to 5s for data
		std::cout << "Attemping connection to " << HOST_NAME << ':' << PORT << "..." << std::endl;
		int received_events = enet_host_service(
			client,
			&event,
			5000		// time to wait in millis -> this sleeps the program
		);
		if (received_events > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			std::cout << "Connection to " << HOST_NAME << ':' << PORT << " succeeded!" << std::endl;
			break;
		}
		else
		{
			//enet_peer_reset(peer);
			std::cout << "Connection to " << HOST_NAME << ':' << PORT << " failed." << std::endl;
		}
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
					if (!id) {
						readBuffer.read<ENetPeer*>(id);
						loginResult.set_value(true);
						std::cout << "You are connected to the server! Your id is " << id << "\n++++++++++++++" << std::endl;
					}
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
					break;
				}
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

	// create an enet client
	ENetHost* client;
	client = enet_host_create(
		NULL,	// the address to bind the server host to, null for client
		1,		// number of outgoing connections, 1 for client
		1,		// number of channels, 1 for now
		0,		// incoming bandwidth, 0 means infinite
		0		// outgoing bandwidth, 0 means infinite
	);

	if (client == NULL)
	{
		std::cout << "Error occurred while trying to create an ENet host." << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::cout << "ENet host created successfully" << std::endl;
	}

	ENetAddress address;
	ENetPeer* peer;

	// set host to connect to
	enet_address_set_host(&address, HOST_NAME);
	address.port = PORT;

	// other program to connect to, in client's case the server
	peer = enet_host_connect(
		client,
		&address,
		1,			// number of channels, 1 for now
		0			// data to send on connect, nothing for now
	);
	if (peer == NULL)
	{
		std::cout << "No available peers for intiating an ENet connection." << std::endl;
	}

	connect(client);

	std::promise<bool> loginResult;
	std::thread read_thread(readNetworkMsgs, client, std::ref(loginResult));
	network::Buffer writeBuffer;
	std::string in;

	// wait for login result
	bool result = loginResult.get_future().get();

	// GAME LOOP START

	while (true) {
		std::getline(std::cin, in);
		messages::body::ChatMsg message;
		message.sender = id;
		std::strcpy(message.message, in.c_str());
		sendMessage<messages::Type::CHAT_MSG, messages::body::ChatMsg>(peer, writeBuffer, message);
	}

	// GAME LOOP END

	read_thread.join();

	enet_peer_disconnect(
		peer,
		0		// data to send on disconnect, nothing for now
	);

	// handle successful disconnection
	ENetEvent event;
	while (enet_host_service(client, &event, 3000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "Disconnection succeeded." << std::endl;
			break;
		default:
		break;
		}
	}

	return EXIT_SUCCESS;
}