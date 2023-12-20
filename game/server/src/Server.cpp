#include "Server.h"

#include <thread>

#include <logging/Logger.h>
#include <network/Server.h>

#include "Game.h"

static constexpr int MAX_NUM_CLIENTS{ 32 };
static constexpr int READ_INTERVAL_MILLIS { 20 };

Server::Server(): _server(common::PORT_NUMBER, MAX_NUM_CLIENTS) {}

void Server::run()
{
	std::thread ping_thread([&]() {
		network::BufferWriter<common::messages::Type> writer;
		auto& buffer = writer.writeToBuffer<common::messages::Type::PING>();
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(common::PING_INTERVAL_MILLIS));
			auto clients = _server.getClients().toList();
			debug::log("[Game Server] Pinging %d clients", clients.size());
			for (auto client : clients)
				network::MessageService::send(client, buffer);
		}
	});
	std::thread read_thread([&]() {
		auto& inMessages = _server.getInQueue();
		network::BufferWriter<common::messages::Type> writer;
		// TODO: add support for multiple games?
		Game game;
		while (true)
		{
			while (!inMessages.empty())
			{
				auto msg = inMessages.popFront();
				auto type = msg.raw.read<common::messages::Type>();
				switch (type)
				{
				case common::messages::Type::PLAYER_JOIN:
				{
					int id = game.addPlayer(msg.source); // TODO: how to handle player disconnection?
					common::messages::JoinDetails body = { id };
					const auto& buffer = writer.writeToBuffer<common::messages::Type::PLAYER_JOIN>(body);
					debug::log("[Game Server] Player joined, assigned id %d", id);
					network::MessageService::send(msg.source, buffer);
				}
				break;
				case common::messages::Type::PLAYER_RECONNECT:
				{
					auto payload = msg.raw.read<common::messages::JoinDetails>();
					game.onReconnect(payload.playerId, msg.source);
				}
				break;
				case common::messages::Type::PLAYER_UPDATE:
				{
					auto update = msg.raw.read<common::messages::PlayerUpdate>();
					const auto& buffer = writer.writeToBuffer<common::messages::Type::PLAYER_UPDATE>(update);
					game.updateOtherPlayers(msg.source, buffer);
				}
				break;
				default:
				break;
				}
			}
		}
	});
	_server.run(READ_INTERVAL_MILLIS);

	ping_thread.join();
	read_thread.join();
}