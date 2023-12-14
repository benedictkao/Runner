#pragma once

#include "PlayerState.h"
#include "input/InputManager.h"

class PlayerManager {
public:		
	PlayerManager(const InputManager&);

public:
	void updatePlayerState();
	void setPlayerOnGround(bool);
	const PlayerState& getPlayerState();

	void setPlayerId(int id);
	int getPlayerId() const;
	
private:
	const InputManager& _input;
	PlayerState _state;	// TODO: make this unordered_map for multiplayer
	int _playerGameId;
};