#pragma once

#include "PlayerState.h"
#include "input/InputManager.h"

class PlayerManager {
private:
	const InputManager& _input;
	PlayerState _state;	// TODO: make this unordered_map for multiplayer

public:
	void updatePlayerState();
	void setPlayerOnGround(bool);
	const PlayerState& getPlayerState();

	PlayerManager(const InputManager&);
};