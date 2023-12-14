#include "PlayerManager.h"

void PlayerManager::updatePlayerState() {
	_state.movement = _input.getState().arrows;
}

void PlayerManager::setPlayerOnGround(bool onGround) {
	_state.onGround = onGround;
}

const PlayerState& PlayerManager::getPlayerState() {
	return _state;
}

void PlayerManager::setPlayerId(int id)
{
	_playerGameId = id;
}

int PlayerManager::getPlayerId() const
{
	return _playerGameId;
}

PlayerManager::PlayerManager(const InputManager& input) : _input(input), _state({ 0,0, false }), _playerGameId(-1) {}
