#pragma once

#include <unordered_map>

#include <entt/entt.hpp>

#include "PlayerRepo.h"
#include "connection/ConnectionManager.h"
#include "input/InputManager.h"
#include "res/TextureRepo.h"
#include "scene/SceneInfo.h"

struct PlayerMetaData
{
	entt::entity entityId;
	int spriteId;
	bool flipHorizontal;
	bool onGround;
};

class PlayerManager {
public:		
	PlayerManager(const InputManager&, const PlayerRepo&);

public:
	void addLocalPlayer(entt::registry&, const PlayerInfo&, TextureRepo&);

	void updatePositions(entt::registry&, ConnectionManager&, TextureRepo&);
	void updateCollisions(entt::registry&);
	void updateSprites(entt::registry&, TextureRepo&);

private:
	void updateLocalPosition(entt::registry&, ConnectionManager&);
	void updateRemotePosition(entt::registry&, int playerId, const PlayerData& remoteData);
	void updateRemotePositions(entt::registry&, TextureRepo&);
	void addPlayer(entt::registry&, int id, const PlayerData&, TextureRepo&);

	void updateCollision(entt::registry&, PlayerMetaData&);
	
private:
	const InputManager& _input;
	const PlayerRepo& _pRepo;
	std::unordered_map<int, PlayerMetaData> _metaData;
};