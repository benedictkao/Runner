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

void Scene::init(const SceneInfo& data) {
	_pManager.addLocalPlayer(_registry, data.playerInfo, _texRepo);

	_background = _registry.create();
	auto bg = _texRepo.loadTexture(data.mapInfo.bg.id);
	_registry.emplace<SpriteComponent>(_background, bg, data.mapInfo.size);

	for (auto& object : data.objects)
	{
		auto platform = _registry.create();
		auto platformTex = _texRepo.loadTexture(object.sprite.id);
		_registry.emplace<TransformComponent>(platform, object.transform);
		_registry.emplace<SpriteComponent>(platform, platformTex, object.sprite.size);
		if (object.hasCollision)
			_registry.emplace<WallComponent>(platform);
		_registry.emplace<TagComponent>(platform, "Platform");
	}


	auto lWall = _registry.create();
	_registry.emplace<TransformComponent>(lWall, -1.0f, 0.0f, 1.0f, data.mapInfo.size.y);
	_registry.emplace<WallComponent>(lWall);
	_registry.emplace<TagComponent>(lWall, "Left Wall");

	auto rWall = _registry.create();
	_registry.emplace<TransformComponent>(rWall, data.mapInfo.size.x, 0.0f, 1.0f, data.mapInfo.size.y);
	_registry.emplace<WallComponent>(rWall);
	_registry.emplace<TagComponent>(rWall, "Right Wall");

	// FIXME: hardcoded floor for now
	auto floor = _registry.create();
	_registry.emplace<TransformComponent>(floor, 0.0f, data.mapInfo.size.y, data.mapInfo.size.x, 1.0f);
	_registry.emplace<WallComponent>(floor);
	_registry.emplace<TagComponent>(floor, "Floor");

	// TEST: hardcode hammer
	auto hammer = _registry.create();
	auto hammerTex = _texRepo.loadTexture(TextureIds::Object::HAMMER);
	_registry.emplace<TransformComponent>(hammer, 200.0f, data.mapInfo.size.y - 64.0f, 32.0f, 64.0f);
	_registry.emplace<WallComponent>(hammer);
	common::Vector2Df hammerSpriteSize = { 32.0f, 64.0f };
	_registry.emplace<SpriteComponent>(hammer, hammerTex, hammerSpriteSize);
	_registry.emplace<AnimationComponent>(hammer, constants::ANIMATION_FRAME_PERIOD, constants::ANIMATION_FRAME_PERIOD * 8);
}

void Scene::update() 
{
	_pManager.update(_registry, _connMgr, _texRepo);
	updateVelocities();
	_pManager.resolveCollisions(_registry);
	updateTransforms();
	updateAnimations();
}

void Scene::blit()
{
	blitBackground();
	blitSprites();
	blitOverlay();
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

void Scene::blitBackground() {
	auto& sprite = _registry.get<SpriteComponent>(_background);
	SDL2::Rect src = { 0, 0, static_cast<int>(constants::WINDOW_WIDTH) * 2, static_cast<int>(constants::WINDOW_HEIGHT) * 2 };
	SDL2::Rect dest = { 0, 0, static_cast<int>(constants::WINDOW_WIDTH), static_cast<int>(constants::WINDOW_HEIGHT) };
	SDL2::blit(_renderer, sprite.tex, src, dest);
}

void Scene::blitOverlay()
{
	auto tex = _texRepo.loadTexture(
		_connMgr.isConnected() ? TextureIds::Icon::CONNECTED : TextureIds::Icon::DISCONNECTED
	);
	int padding = 12;
	int iconSize = 24;
	int y = static_cast<int>(constants::WINDOW_HEIGHT) - iconSize - padding;
	SDL2::Rect dest = { padding, y, iconSize, iconSize };
	SDL2::blit(_renderer, tex, dest);
}

void Scene::blitSprites() {
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