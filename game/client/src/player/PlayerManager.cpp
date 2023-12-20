#include "PlayerManager.h"

#include "Components.h"
#include "res/ResIds.h"

static constexpr auto PLAYER_SPEED { 6.0f };	// TODO: make this customisable
static constexpr auto JUMP_SPEED{ 24.0f };		// TODO: make this customisable
static constexpr auto ANIMATION_PERIOD{ 4 };	// TODO: make this customisable
static constexpr unsigned int DEFAULT_SPRITE_ID { SpriteIds::PINK_MONSTER };


void PlayerManager::setPlayerOnGround(bool onGround) {
	_playerMetaData.onGround = onGround;
}

void PlayerManager::setEntityId(entt::entity id)
{
	_playerMetaData.entityId = id;
}

void PlayerManager::setSpriteId(unsigned int id)
{
	_playerMetaData.spriteId = id;
}

unsigned int PlayerManager::getSpriteId() const
{
	return _playerMetaData.spriteId;
}

PlayerManager::PlayerManager(const InputManager& input, const PlayerRepo& pRepo):
	_input(input),
	_pRepo(pRepo),
	_playerMetaData({ entt::null, SpriteIds::PINK_MONSTER, false })
{}

void PlayerManager::updatePositions(entt::registry& registry, ConnectionManager& connMgr)
{
	entt::entity player = _playerMetaData.entityId;

	const auto& inputArrows = _input.getState().arrows;
	auto& velo = registry.get<VelocityComponent>(player);
	velo.vector.x = inputArrows.x * PLAYER_SPEED;
	if (velo.vector.x != 0)
		_playerMetaData.flipHorizontal = velo.vector.x < 0;

	if (inputArrows.y > 0 && _playerMetaData.onGround)
	{
		registry.get<AnimationComponent>(player).current = -1;	// will be incremented later
		velo.vector.y = -JUMP_SPEED;
	}

	const common::messages::PlayerUpdate update = { 
		_pRepo.getPlayerId(), 
		_playerMetaData.spriteId, 
		registry.get<TransformComponent>(player).rect, 
		registry.get<VelocityComponent>(player).vector 
	};
	connMgr.send<common::messages::Type::PLAYER_UPDATE>(update);
}

void PlayerManager::updateSprites(entt::registry& registry, TextureRepo& texRepo)
{
	entt::entity player = _playerMetaData.entityId;

	const auto& [sprite, animation] = registry.get<SpriteComponent, AnimationComponent>(player);
	auto& horizontalMovement = registry.get<VelocityComponent>(player).vector.x;
	auto spritePack = SpriteIds::getPack(_playerMetaData.spriteId);

	if (_playerMetaData.onGround) 
	{
		if (horizontalMovement != 0) {
			sprite.tex = texRepo.loadTexture(spritePack.run);
			animation.wavelength = ANIMATION_PERIOD * 6;
		}
		else {
			sprite.tex = texRepo.loadTexture(spritePack.idle);
			animation.wavelength = ANIMATION_PERIOD * 4;
		}
	}
	else 
	{
		sprite.tex = texRepo.loadTexture(spritePack.jump);
		animation.wavelength = ANIMATION_PERIOD * 8;
	}
	sprite.flipHorizontal = _playerMetaData.flipHorizontal;
}
