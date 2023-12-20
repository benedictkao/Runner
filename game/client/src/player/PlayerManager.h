#pragma once

#include <entt/entt.hpp>

#include "PlayerState.h"
#include "PlayerRepo.h"
#include "connection/ConnectionManager.h"
#include "input/InputManager.h"
#include "res/TextureRepo.h"

struct PlayerMetaData
{
	entt::entity entityId;
	unsigned int spriteId;
	bool flipHorizontal;
	bool onGround;
};

class PlayerManager {
public:		
	PlayerManager(const InputManager&, const PlayerRepo&);

public:
	void updatePositions(entt::registry&, ConnectionManager&);
	void updateSprites(entt::registry&, TextureRepo&);

	void setPlayerOnGround(bool);

	void setEntityId(entt::entity);

	void setSpriteId(unsigned int id);
	unsigned int getSpriteId() const;
	
private:
	const InputManager& _input;
	const PlayerRepo& _pRepo;
	PlayerMetaData _playerMetaData;
};