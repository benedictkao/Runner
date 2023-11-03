#include "Scene.h"
#include "Components.h"
#include "SDL.h"
#include "math/Math.h"
#include <iostream>
#include <vector>

constexpr auto WINDOW_WIDTH{ 800.0f };
constexpr auto WINDOW_HEIGHT{ 648.0f };
constexpr auto PLAYER_SPEED{ 6.0f };
constexpr auto JUMP_SPEED{ 24.0f };
constexpr auto GRAVITY_ACCEL{ 2.0f };
constexpr auto TERMINAL_DROP_VELO{ 20.0f };
constexpr auto ANIMATION_PERIOD{ 4 };

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

	_registry.emplace<TransformComponent>(_player, 100.0f, 200.0f, 56.0f, 88.0f);
	_registry.emplace<SpriteComponent>(_player, sprite, 128, 128);
	_registry.emplace<AnimationComponent>(_player, ANIMATION_PERIOD, ANIMATION_PERIOD * 6);
	_registry.emplace<VelocityComponent>(_player);
	_registry.emplace<GravityComponent>(_player);
	_registry.emplace<TagComponent>(_player, "Player");

	_background = _registry.create();
	auto bg = _texRepo.loadTexture(TextureIds::BACKGROUND);
	_registry.emplace<SpriteComponent>(_background, bg, 2304, 1296);

	auto floor = _registry.create();
	_registry.emplace<TransformComponent>(floor, 0.0f, WINDOW_HEIGHT, WINDOW_WIDTH, 1.0f);
	_registry.emplace<WallComponent>(floor);
	_registry.emplace<TagComponent>(floor, "Floor");

	auto lWall = _registry.create();
	_registry.emplace<TransformComponent>(lWall, -1.0f, 0.0f, 1.0f, WINDOW_HEIGHT);
	_registry.emplace<WallComponent>(lWall);
	_registry.emplace<TagComponent>(lWall, "Left Wall");
	auto rWall = _registry.create();
	_registry.emplace<TransformComponent>(rWall, WINDOW_WIDTH, 0.0f, 1.0f, WINDOW_HEIGHT);
	_registry.emplace<WallComponent>(rWall);
	_registry.emplace<TagComponent>(rWall, "Right Wall");

	auto platformTex = _texRepo.loadTexture(TextureIds::TILE);
	for (int i = 0; i < 5; i++) {
		auto platform = _registry.create();
		_registry.emplace<TransformComponent>(platform, 200.0f + 32 * i, WINDOW_HEIGHT - 128, 32.0f, 32.0f);
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
	updateSprites();
	updateAnimations();
}

void Scene::updateBackground() {
	auto& sprite = _registry.get<SpriteComponent>(_background);
	SDL2::Rect src = { 0, 0, 1600, 1296 };
	SDL2::Rect dest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
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
			sprite.padding = { 18, 54, 40, 0 };
			sprite.flipHorizontal = currMovement < 0;
			animation.wavelength = ANIMATION_PERIOD * 8;
		}
		else {
			sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_IDLE);
			sprite.padding = { 36, 36, 40, 0 };
			animation.wavelength = ANIMATION_PERIOD * 6;
		}

		if (_pControl.isJumping())
			velo.vector.y = -JUMP_SPEED;
	}
	else {
		sprite.tex = _texRepo.loadTexture(TextureIds::PLAYER_JUMP);
		sprite.padding = { 26, 48, 40, 0 };
		if (currMovement != 0) {
			sprite.flipHorizontal = currMovement < 0;
		}
		animation.wavelength = 0;
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
		velo.vector.y += GRAVITY_ACCEL;
		math::coerceAtMost(velo.vector.y, TERMINAL_DROP_VELO);
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
		sprite.pos.x = spritePos * sprite.size.x;

		++animation.current;
	}
}

void Scene::updateSprites() {
	auto& view = _registry.view<SpriteComponent, TransformComponent>();
	for (auto entity : view) {
		const auto& [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
		SDL2::Rect dest = math::toSDLRect(transform.rect);
		//if (entity == _player)
		//	std::cout << dest.y << std::endl;
		int srcX = sprite.pos.x + sprite.padding.left;
		int srcY = sprite.pos.y + sprite.padding.top;
		int srcW = sprite.size.x - sprite.padding.left - sprite.padding.right;
		int srcH = sprite.size.y - sprite.padding.top - sprite.padding.bottom;
		SDL2::Rect src = { srcX, srcY, srcW, srcH };
		SDL2::blit(_renderer, sprite.tex, src, dest, sprite.flipHorizontal);
	}
}