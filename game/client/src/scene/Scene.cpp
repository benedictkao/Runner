#include "Scene.h"

#include <logging/Logger.h>
#include <vector>

#include "Components.h"
#include "Constants.h"
#include "sdl/SDL.h"
#include "math/Math.h"

constexpr auto PLAYER_SPEED{ 6.0f };
constexpr auto JUMP_SPEED{ 24.0f };
constexpr auto ANIMATION_PERIOD{ 4 };

Scene::Scene(
	SDL2::Renderer renderer,
	PlayerManager& pManager,
	TextureRepo& texRepo,
	ConnectionManager& connMgr
) : _renderer(renderer), _pManager(pManager), _texRepo(texRepo), _connMgr(connMgr) {}

void Scene::init(SceneInfo data) {
	_player = _registry.create();

	auto sprite = _texRepo.loadTexture(TextureIds::PLAYER_IDLE);
	_texRepo.loadTexture(TextureIds::PLAYER_RUN);
	_texRepo.loadTexture(TextureIds::PLAYER_JUMP);

	_registry.emplace<TransformComponent>(_player, data.playerInfo.transform);
	_registry.emplace<SpriteComponent>(_player, sprite, 32, 32, 2)
		.offset = { 6, 4 };
	_registry.emplace<AnimationComponent>(_player, ANIMATION_PERIOD, ANIMATION_PERIOD * 6);
	_registry.emplace<VelocityComponent>(_player);
	_registry.emplace<GravityComponent>(_player);
	_registry.emplace<TagComponent>(_player, "Player");

	_background = _registry.create();
	auto bg = _texRepo.loadTexture(data.mapInfo.bg.texId);
	_registry.emplace<SpriteComponent>(_background, bg, data.mapInfo.size);

	for (auto& object : data.objects)
	{
		auto platform = _registry.create();
		auto platformTex = _texRepo.loadTexture(object.sprite.texId);
		_registry.emplace<TransformComponent>(platform, object.transform);
		_registry.emplace<SpriteComponent>(platform, platformTex, object.sprite.size);
		if (object.hasCollision)
			_registry.emplace<WallComponent>(platform);
		_registry.emplace<TagComponent>(platform, "Platform");
	}

	if (data.mapInfo.walls.left)
	{
		auto lWall = _registry.create();
		_registry.emplace<TransformComponent>(lWall, -1.0f, 0.0f, 1.0f, data.mapInfo.size.y);
		_registry.emplace<WallComponent>(lWall);
		_registry.emplace<TagComponent>(lWall, "Left Wall");
	}
	if (data.mapInfo.walls.right)
	{
		auto rWall = _registry.create();
		_registry.emplace<TransformComponent>(rWall, constants::WINDOW_WIDTH, 0.0f, 1.0f, data.mapInfo.size.x);
		_registry.emplace<WallComponent>(rWall);
		_registry.emplace<TagComponent>(rWall, "Right Wall");
	}
}

void Scene::updateLogic() 
{
	updatePlayer();
	updateVelocities();
	updateCollisions();
	updateTransforms();
}

void Scene::updateTextures()
{
	updateBackground();
	updateAnimations();
	updateSprites();
	updateOverlay();
}

void Scene::updateBackground() {
	auto& sprite = _registry.get<SpriteComponent>(_background);
	SDL2::Rect src = { 0, 0, static_cast<int>(constants::WINDOW_WIDTH) * 2, static_cast<int>(constants::WINDOW_HEIGHT) * 2 };
	SDL2::Rect dest = { 0, 0, static_cast<int>(constants::WINDOW_WIDTH), static_cast<int>(constants::WINDOW_HEIGHT) };
	SDL2::blit(_renderer, sprite.tex, src, dest);
}

void Scene::updatePlayer() {
	_pManager.updatePlayerState();
	const auto& [sprite, animation] = _registry.get<SpriteComponent, AnimationComponent>(_player);
	int currMovement = _pManager.getPlayerState().movement.x;
	auto& velo = _registry.get<VelocityComponent>(_player);
	velo.vector.x = currMovement * PLAYER_SPEED;

	if (_pManager.getPlayerState().onGround) {
		if (currMovement != 0) {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_RUN);
			sprite.flipHorizontal = currMovement < 0;
			animation.wavelength = ANIMATION_PERIOD * 6;
		}
		else {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_IDLE);
			animation.wavelength = ANIMATION_PERIOD * 4;
		}

		if (_pManager.getPlayerState().movement.y > 0) {
			velo.vector.y = -JUMP_SPEED;

			animation.current = 0;
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_JUMP);
			if (currMovement != 0) {
				sprite.flipHorizontal = currMovement < 0;
			}
			animation.wavelength = ANIMATION_PERIOD * 8;
		}
	}
	else {
		sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_JUMP);
		if (currMovement != 0) {
			sprite.flipHorizontal = currMovement < 0;
		}
		animation.wavelength = ANIMATION_PERIOD * 8;
	}
}

void Scene::updateTransforms() {
	const auto& view = _registry.view<VelocityComponent, TransformComponent>();
	for (auto entity : view) {
		const auto& [velo, transform] = view.get(entity);
		transform.rect.pos += velo.vector;
	}
}

void Scene::updateVelocities() {
	const auto& view = _registry.view<VelocityComponent, GravityComponent>();
	for (auto entity : view) {
		auto& velo = view.get<VelocityComponent>(entity);
		velo.vector.y += constants::GRAVITY_ACCEL;
		math::coerceAtMost(velo.vector.y, constants::TERMINAL_DROP_VELO);
	}
}

void Scene::updateCollisions() {
	const auto& pRect = _registry.get<TransformComponent>(_player).rect;
	auto& pMovement = _registry.get<VelocityComponent>(_player).vector;
	const auto& walls = _registry.view<TransformComponent, WallComponent>();

	struct Collision {
		entt::entity entity;
		float contactTime;
		bool isDiagonal;
	};

	std::vector<Collision> collisions;
	for (auto entity : walls) {
		const auto& wallRect = walls.get<TransformComponent>(entity).rect;
		Vector2Df contactPoint;
		Vector2Df contactNormal;
		float contactTime;
		if (math::sweptRectVsRect(pRect, pMovement, wallRect, contactPoint, contactNormal, contactTime)) {
			// only diagonal collision will have both x and y as non-zero
			bool isDiagonal = static_cast<bool>(contactNormal.x * contactNormal.y);
			collisions.push_back({entity, contactTime, isDiagonal});
		}
	}
	std::sort(collisions.begin(), collisions.end(),
		[](const Collision& a, const Collision& b) {
			if (a.contactTime == b.contactTime) {
				// resolve whichever is not diagonal first
				return a.isDiagonal < b.isDiagonal;
			}
			else {
				return a.contactTime < b.contactTime;
			}
		}
	);

	bool onGround = false;
	for (const auto& collision : collisions) {
		// player is not moving, no need to calculate further collisions
		if (pMovement.x == 0 && pMovement.y == 0)
			break;

		const auto& wallRect = walls.get<TransformComponent>(collision.entity).rect;
		const Vector2Df contactNormal = math::resolveSweptRectVsRect(pRect, pMovement, wallRect);
		if (contactNormal.y < 0) {
			onGround = true;
		}
		//if (contactNormal.x != 0) {
		//	debug::log("contactNormal = %f, %f", contactNormal.x, contactNormal.y);
		//}
	}
	_pManager.setPlayerOnGround(onGround);
}

void Scene::updateAnimations() {
	const auto& view = _registry.view<AnimationComponent, SpriteComponent>();
	for (auto entity : view) {
		const auto& [animation, sprite] = view.get(entity);
		if (animation.current >= animation.wavelength)
			animation.current = 0;

		int spritePos = animation.current / animation.period;
		sprite.srcRect.pos.x = spritePos * sprite.srcRect.size.x;

		++animation.current;
	}
}

void Scene::updateOverlay()
{
	auto tex = _texRepo.loadTexture(
		_connMgr.isConnected() ? TextureIds::ICON_CONNECTED : TextureIds::ICON_DISCONNECTED
	);
	int padding = 12;
	int iconSize = 24;
	int y = constants::WINDOW_HEIGHT - iconSize - padding;
	SDL2::Rect dest = { padding, y, iconSize, iconSize };
	SDL2::blit(_renderer, tex, dest);
}

void Scene::updateSprites() {
	const auto& view = _registry.view<SpriteComponent, TransformComponent>();
	for (auto entity : view) {
		const auto& [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
		const auto& size = sprite.srcRect.size;

		// if flipped, need to find offset of other side of sprite pack
		float offsetX = sprite.flipHorizontal ? (size.x - sprite.offset.x) * sprite.scale - transform.rect.size.x : sprite.offset.x * sprite.scale;

		SDL2::Rect dest = { static_cast<int>(transform.rect.pos.x - offsetX),
			static_cast<int>(transform.rect.pos.y - sprite.offset.y * sprite.scale),
			static_cast<int>(size.x * sprite.scale),
			static_cast<int>(size.y * sprite.scale) };

		const auto& srcPos = sprite.srcRect.pos;
		SDL2::Rect src = { static_cast<int>(srcPos.x), static_cast<int>(srcPos.y), static_cast<int>(size.x), static_cast<int>(size.y) };
		SDL2::blit(_renderer, sprite.tex, src, dest, sprite.flipHorizontal);
	}
}