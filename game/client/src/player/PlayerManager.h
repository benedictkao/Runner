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

	void update(entt::registry&, ConnectionManager&, TextureRepo&);
	void resolveCollisions(entt::registry&);

private:
	void updateLocalPosition(entt::registry&, ConnectionManager&);
	void updateRemotePosition(entt::registry&, int playerId, const PlayerData& remoteData);
	void updateRemotePositions(entt::registry&, TextureRepo&);
	void updateSprites(entt::registry&, TextureRepo&);

	void addPlayer(entt::registry&, int id, const PlayerData&, TextureRepo&);

	void resolveCollision(entt::registry&, PlayerMetaData&);
	
private:
	const InputManager& _input;
	const PlayerRepo& _pRepo;
	std::unordered_map<int, PlayerMetaData> _metaData;
};