#include "PlayerManager.h"

#include "res/ResIds.h"

static constexpr unsigned int DEFAULT_SPRITE_ID { SpriteIds::PINK_MONSTER };

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

void PlayerManager::setSpriteId(unsigned int id)
{
	_spriteId = id;
}

unsigned int PlayerManager::getSpriteId() const
{
	return _spriteId;
}

PlayerManager::PlayerManager(const InputManager& input):
	_input(input),
	_state({ 0, 0, false }), 
	_playerGameId(-1), 
	_spriteId(DEFAULT_SPRITE_ID) 
{}