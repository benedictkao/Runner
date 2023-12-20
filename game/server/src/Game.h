#pragma once

#include <vector>
#include <network/Common.h>

enum class GameState
{
	WAITING, ONGOING, FINISHED
};

class Game
{
public:
	Game();

public:
	int addPlayer(ENetPeer*);
	void onReconnect(int playerId, ENetPeer* peer);
	void onDisconnect(int playerId);
	void updateOtherPlayers(ENetPeer* source, const network::Buffer& payload);

private:
	std::vector<ENetPeer*> _players;
	GameState _state;
};