#include "PlayerManager.h"

#include "Constants.h"
#include "component/Components.h"
#include "math/Math.h"
#include "res/ResIds.h"

static constexpr unsigned int DEFAULT_SPRITE_ID { SpriteIds::PINK_MONSTER };

static constexpr int LOCAL_PLAYER_ID { -1 };

void PlayerManager::updateLocalPosition(entt::registry& registry, ConnectionManager& connMgr)
{
	auto& localPlayer = _metaData[LOCAL_PLAYER_ID];
	entt::entity entityId = localPlayer.entityId;

	const auto& inputArrows = _input.getState().arrows;
	auto& velo = registry.get<VelocityComponent>(entityId);
	velo.vector.x = inputArrows.x * constants::BASE_PLAYER_MOVE_SPEED;
	if (velo.vector.x != 0)
		localPlayer.flipHorizontal = velo.vector.x < 0;

	if (inputArrows.y > 0 && localPlayer.onGround)
	{
		registry.get<AnimationComponent>(entityId).current = -1;	// will be incremented later
		velo.vector.y = -constants::BASE_PLAYER_JUMP_SPEED;
	}

	auto gameId = _pRepo.getPlayerId();
	if (gameId != Player::DEFAULT_ID)
	{
		const common::messages::PlayerUpdate update = { 
			gameId, 
			localPlayer.spriteId, 
			registry.get<TransformComponent>(entityId).rect, 
			registry.get<VelocityComponent>(entityId).vector 
		};
		connMgr.send<common::messages::Type::PLAYER_UPDATE>(update);
	}
}

void PlayerManager::updateRemotePosition(entt::registry& registry, int playerId, const PlayerData& remoteData)
{
	auto& playerData = _metaData[playerId];
	playerData.spriteId = remoteData.spriteId;
	if (remoteData.speed.x != 0)
		playerData.flipHorizontal = remoteData.speed.x < 0;
	registry.get<TransformComponent>(playerData.entityId).rect = remoteData.transform;
	registry.get<VelocityComponent>(playerData.entityId).vector = remoteData.speed;
}

void PlayerManager::updateRemotePositions(entt::registry& registry, TextureRepo& texRepo)
{
	const auto& allData = _pRepo.getRemoteData();
	auto end = allData.size();
	for (int playerId = 0; playerId < end; ++playerId)
	{
		const auto& remoteData = allData[playerId];
		if (!remoteData.connected)
			continue;

		if (_metaData.count(playerId) == 0)
			addPlayer(registry, playerId, remoteData, texRepo);
		else
			updateRemotePosition(registry, playerId, remoteData);
	}
}

PlayerManager::PlayerManager(const InputManager& input, const PlayerRepo& pRepo):
	_input(input),
	_pRepo(pRepo)
{}

void PlayerManager::addLocalPlayer(entt::registry& registry, const PlayerInfo& playerInfo, TextureRepo& texRepo)
{
	int spriteId = DEFAULT_SPRITE_ID;	// FIXME: should not be hardcoded
	addPlayer(registry, LOCAL_PLAYER_ID, { spriteId, playerInfo.transform, { 0.0f, 0.0f } }, texRepo);
}

void PlayerManager::addPlayer(entt::registry& registry, int id, const PlayerData& data, TextureRepo& texRepo)
{
	const auto spritePack = SpriteIds::getPack(data.spriteId);
	auto idle = texRepo.loadTexture(spritePack.idle);
	texRepo.loadTexture(spritePack.run);
	texRepo.loadTexture(spritePack.jump);

	auto player = registry.create();
	registry.emplace<TransformComponent>(player, data.transform);
	// float scale = playerInfo.transform.size.x / spritePack.size.x;
	registry.emplace<SpriteComponent>(player, idle, spritePack.size, 2)	// TODO: fix hardcoded scale value
		.offset = spritePack.offset;
	registry.emplace<AnimationComponent>(player, constants::ANIMATION_FRAME_PERIOD, constants::ANIMATION_FRAME_PERIOD * 6);
	registry.emplace<VelocityComponent>(player);
	registry.emplace<GravityComponent>(player);
	registry.emplace<TagComponent>(player, "Player");

	_metaData[id] = { player, data.spriteId, false, false };
}

void PlayerManager::update(entt::registry& registry, ConnectionManager& connMgr, TextureRepo& texRepo)
{
	updateLocalPosition(registry, connMgr);
	updateRemotePositions(registry, texRepo);
	updateSprites(registry, texRepo);
}

void PlayerManager::resolveCollisions(entt::registry& registry)
{
	for (auto& pair : _metaData)
		resolveCollision(registry, pair.second);
}

void PlayerManager::resolveCollision(entt::registry& registry, PlayerMetaData& playerData)
{
	auto player = playerData.entityId;

	auto& pRect = registry.get<TransformComponent>(player).rect;
	auto& pMovement = registry.get<VelocityComponent>(player).vector;
	const auto& walls = registry.view<TransformComponent, CollisionComponent>();
	const auto& animWalls = registry.view<TransformComponent, AnimationComponent, AnimatedColliderComponent>();

	struct Collision
	{
		entt::entity entity;
		float contactTime;
		bool isDiagonal;
		common::Vector2Df wallVelo;
		common::Vector2Df wallDisplacement;
	};

	std::vector<Collision> collisions;

	for (auto wall : walls)
	{
		const auto& wallRect = walls.get<TransformComponent>(wall).rect;
		common::Vector2Df contactPoint;
		common::Vector2Df contactNormal;
		float contactTime;

		if (math::sweptRectVsRect(pRect, pMovement, wallRect, contactPoint, contactNormal, contactTime))
		{
			// only diagonal collision will have both x and y as non-zero
			bool isDiagonal = contactNormal.y != 0 && contactNormal.x != 0;
			collisions.push_back({wall, contactTime, isDiagonal, { 0.0f, 0.0f }, { 0.0f, 0.0f }});
		}
	}

	for (auto animWall : animWalls)
	{
		const auto& [transform, anim, collider] = animWalls.get(animWall);

		common::Vector2Df contactPoint;
		common::Vector2Df contactNormal;
		float contactTime;
		
		// adjust player velo relative to wall
		const common::Vector2Df dir = collider.isVertical ? common::Vector2Df(0.0f, 1.0f) : common::Vector2Df(1.0f, 0.0f);

		auto currDisplacement = collider.displacements[anim.current / anim.period];
		int next = anim.current + 1;
		next = next >= anim.wavelength ? 0 : next;
		auto speed = collider.displacements[next / anim.period] - currDisplacement;
		const common::Vector2Df wallVelo = dir * speed;
		pMovement -= wallVelo;

		common::Rect2Df target = transform.rect;
		auto wallDisplacement = dir * currDisplacement;
		target.pos += wallDisplacement;
		if (math::sweptRectVsRect(pRect, pMovement, target, contactPoint, contactNormal, contactTime))
		{
			// only diagonal collision will have both x and y as non-zero
			bool isDiagonal = contactNormal.y != 0 && contactNormal.x != 0;
			collisions.push_back({animWall, contactTime, isDiagonal, wallVelo, wallDisplacement});
		}

		// revert player velo
		pMovement += wallVelo;
	}

	std::sort(collisions.begin(), collisions.end(),
		[](const Collision& a, const Collision& b) 
		{
			if (a.contactTime == b.contactTime) 
			{
				// resolve whichever is NOT diagonal first
				// comparator must return false for equal elements: https://stackoverflow.com/questions/65468629/stl-sort-debug-assertion-failed
				return a.isDiagonal < b.isDiagonal;
			}
			else 
			{
				return a.contactTime < b.contactTime;
			}
		}
	);

	bool onGround = false;
	for (const auto& collision : collisions) {
		// player is not moving, no need to calculate further collisions
		if (pMovement.x == 0 && pMovement.y == 0)
			break;

		auto target = walls.get<TransformComponent>(collision.entity).rect;
		target.pos += collision.wallDisplacement;
		
		// adjust player velo relative to wall
		pMovement -= collision.wallVelo;

		const common::Vector2Df contactNormal = math::resolveSweptRectVsRect(pRect, pMovement, target);
		if (contactNormal.y < 0)
			onGround = true;

		/*
		* FIX ME!!!!
		* 
		* Player is "jumping" up when being pushed up because wallVelo is being added to playerVelo. wallVelo is updated instantaneously
		* by AnimatedColliderComponent logic, but player velo remains.
		* 
		* Potential solution: instead of adding wall velo back to pMovement, can this be acted on the transform directly?
		*/

		//pMovement += collision.wallVelo;
		pRect.pos += collision.wallVelo;
	}
	playerData.onGround = onGround;
}

void PlayerManager::updateSprites(entt::registry& registry, TextureRepo& texRepo)
{
	for (const auto& pair : _metaData)
	{
		entt::entity player = pair.second.entityId;
		const auto& [sprite, animation] = registry.get<SpriteComponent, AnimationComponent>(player);
		auto& horizontalMovement = registry.get<VelocityComponent>(player).vector.x;
		auto spritePack = SpriteIds::getPack(pair.second.spriteId);

		if (pair.second.onGround) 
		{
			if (horizontalMovement != 0)
			{
				sprite.tex = texRepo.loadTexture(spritePack.run);
				animation.wavelength = constants::ANIMATION_FRAME_PERIOD * 6;
			}
			else
			{
				sprite.tex = texRepo.loadTexture(spritePack.idle);
				animation.wavelength = constants::ANIMATION_FRAME_PERIOD * 4;
			}
		}
		else
		{
			sprite.tex = texRepo.loadTexture(spritePack.jump);
			animation.wavelength = constants::ANIMATION_FRAME_PERIOD * 8;
		}
		sprite.flipHorizontal = pair.second.flipHorizontal;
	}
}