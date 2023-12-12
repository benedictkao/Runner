#include "Game.h"

Game::Game(): _state(GameState::WAITING) {}

int Game::addPlayer(ENetPeer* peer)
{
	int id = _players.size();
	_players.push_back(peer);
	return id;
}

void Game::onReconnect(int playerId, ENetPeer* peer)
{
	_players[playerId] = peer;
}
