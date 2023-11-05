#include "Scene.h"
#include "Components.h"
#include "Constants.h"
#include "SDL.h"
#include "math/Math.h"
#include <iostream>
#include <vector>

constexpr auto PLAYER_SPEED{ 6.0f };
constexpr auto JUMP_SPEED{ 24.0f };
constexpr auto ANIMATION_PERIOD{ 4 };
constexpr auto TILE_SIZE{ 32.0f };

Scene::Scene(
	SDL2::Renderer renderer,
	PlayerController& pControl,
	TextureRepo& texRepo
) : _renderer(renderer), _pControl(pControl), _texRepo(texRepo) {}

void Scene::init() {
	_player = _registry.create();

	auto sprite = _texRepo.loadTexture(TextureIds::PLAYER_IDLE);
	_texRepo.loadTexture(TextureIds::PLAYER_RUN);
	_texRepo.loadTexture(TextureIds::PLAYER_JUMP);
	_texRepo.loadTexture(TextureIds::PLAYER_FALL);

	_registry.emplace<TransformComponent>(_player, 100.0f, 200.0f, 36.0f, 80.0f);
	_registry.emplace<SpriteComponent>(_player, sprite, 128, 128);
	_registry.emplace<AnimationComponent>(_player, ANIMATION_PERIOD, ANIMATION_PERIOD * 6);
	_registry.emplace<VelocityComponent>(_player);
	_registry.emplace<GravityComponent>(_player);
	_registry.emplace<TagComponent>(_player, "Player");

	_background = _registry.create();
	auto bg = _texRepo.loadTexture(TextureIds::BACKGROUND);
	_registry.emplace<SpriteComponent>(_background, bg, 2304, 1296);

	auto platformTex = _texRepo.loadTexture(TextureIds::TILE);
	int numTiles = constants::WINDOW_WIDTH / TILE_SIZE;
	for (int i = 0; i < numTiles; ++i) {
		auto floor = _registry.create();
		_registry.emplace<TransformComponent>(floor, TILE_SIZE * i, constants::WINDOW_HEIGHT - TILE_SIZE, TILE_SIZE, TILE_SIZE);
		_registry.emplace<SpriteComponent>(floor, platformTex, 32, 32);
		_registry.emplace<WallComponent>(floor);
		_registry.emplace<TagComponent>(floor, "Floor");
	}

	auto lWall = _registry.create();
	_registry.emplace<TransformComponent>(lWall, -1.0f, 0.0f, 1.0f, constants::WINDOW_HEIGHT);
	_registry.emplace<WallComponent>(lWall);
	_registry.emplace<TagComponent>(lWall, "Left Wall");
	auto rWall = _registry.create();
	_registry.emplace<TransformComponent>(rWall, constants::WINDOW_WIDTH, 0.0f, 1.0f, constants::WINDOW_HEIGHT);
	_registry.emplace<WallComponent>(rWall);
	_registry.emplace<TagComponent>(rWall, "Right Wall");

	for (int i = 0; i < 5; ++i) {
		auto platform = _registry.create();
		_registry.emplace<TransformComponent>(platform, 200.0f + TILE_SIZE * i, constants::WINDOW_HEIGHT - 164, TILE_SIZE, TILE_SIZE);
		_registry.emplace<SpriteComponent>(platform, platformTex, 32, 32);
		_registry.emplace<WallComponent>(platform);
		_registry.emplace<TagComponent>(platform, "Platform");
	}
}

void Scene::update() {
	updateBackground();
	updatePlayer();
	updateVelocities();
	updateCollisions();
	updateTransforms();
	updateAnimations();
	updateSprites();
}

void Scene::updateBackground() {
	auto& sprite = _registry.get<SpriteComponent>(_background);
	SDL2::Rect src = { 0, 0, constants::WINDOW_WIDTH * 2, constants::WINDOW_HEIGHT * 2 };
	SDL2::Rect dest = { 0, 0, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT };
	SDL2::blit(_renderer, sprite.tex, src, dest);
}

void Scene::updatePlayer() {
	auto& [sprite, animation] = _registry.get<SpriteComponent, AnimationComponent>(_player);
	int currMovement = _pControl.getCurrentMovement();
	auto& velo = _registry.get<VelocityComponent>(_player);
	velo.vector.x = currMovement * PLAYER_SPEED;

	if (_pControl.isOnGround()) {
		if (currMovement != 0) {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_RUN);
			sprite.offset = { 32, 48 };
			sprite.flipHorizontal = currMovement < 0;
			animation.wavelength = ANIMATION_PERIOD * 8;
		}
		else {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_IDLE);
			sprite.offset = { 46, 48 };
			animation.wavelength = ANIMATION_PERIOD * 6;
		}

		if (_pControl.isJumping()) {
			velo.vector.y = -JUMP_SPEED;
		}
	}
	else {
		if (velo.vector.y > 0) {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_FALL);
		}
		else {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_JUMP);
		}
		sprite.offset = { 44, 48 };
		if (currMovement != 0) {
			sprite.flipHorizontal = currMovement < 0;
		}
		animation.wavelength = 1;
	}
}

void Scene::updateTransforms() {
	auto& view = _registry.view<VelocityComponent, TransformComponent>();
	for (auto entity : view) {
		auto& [velo, transform] = view.get(entity);
		transform.rect.pos += velo.vector;
	}
}

void Scene::updateVelocities() {
	auto& view = _registry.view<VelocityComponent, GravityComponent>();
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
	};

	std::vector<Collision> collisions;
	for (auto entity : walls) {
		const auto& wallRect = walls.get<TransformComponent>(entity).rect;
		Vector2Df contactPoint;
		Vector2Df contactNormal;
		float contactTime;
		if (math::sweptRectVsRect(pRect, pMovement, wallRect, contactPoint, contactNormal, contactTime)) {
			collisions.push_back({entity, contactTime});
		}
	}
	std::sort(collisions.begin(), collisions.end(),
		[](const Collision& a, const Collision& b) {
			return a.contactTime < b.contactTime;
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
	}
	_pControl.setOnGround(onGround);
}

void Scene::updateAnimations() {
	auto& view = _registry.view<AnimationComponent, SpriteComponent>();
	for (auto entity : view) {
		auto& [animation, sprite] = view.get(entity);
		if (animation.current >= animation.wavelength)
			animation.current = 0;

		int spritePos = animation.current / animation.period;
		sprite.srcRect.pos.x = spritePos * sprite.srcRect.size.x;

		++animation.current;
	}
}

void Scene::updateSprites() {
	auto& view = _registry.view<SpriteComponent, TransformComponent>();
	for (auto entity : view) {
		const auto& [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
		float w = sprite.srcRect.size.x;
		float h = sprite.srcRect.size.y;

		// if flipped, need to find offset of other side of sprite pack
		float offsetX = sprite.flipHorizontal ? sprite.srcRect.size.x - transform.rect.size.x - sprite.offset.x : sprite.offset.x;

		SDL2::Rect dest = { static_cast<int>(transform.rect.pos.x - offsetX),
			static_cast<int>(transform.rect.pos.y - sprite.offset.y),
			static_cast<int>(w),
			static_cast<int>(h) };

		//if (entity == _player)
		//	std::cout << dest.y << std::endl;
		const auto& srcPos = sprite.srcRect.pos;
		SDL2::Rect src = { srcPos.x, srcPos.y, w, h };
		SDL2::blit(_renderer, sprite.tex, src, dest, sprite.flipHorizontal);
	}
}