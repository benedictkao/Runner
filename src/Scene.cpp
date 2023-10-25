#include "Scene.h"
#include "Components.h"
#include "SDL.h"

Scene::Scene(SDL2::Renderer renderer) : _renderer(renderer) {}

void Scene::init() {
	_player = _registry.create();
	_registry.emplace<TransformComponent>(_player, 100, 500, 128, 128);
	SDL2::Texture sprite = SDL2::loadTexture(_renderer, "../../../res/texture/sprite/Idle.png");
	_registry.emplace<SpriteComponent>(_player, sprite, 0, 0);
	_registry.emplace<AnimationComponent>(_player, 2, 0, 12);

	_background = _registry.create();
	SDL2::Texture bg = SDL2::loadTexture(_renderer, "../../../res/texture/background/4.png");
	_registry.emplace<SpriteComponent>(_background, bg, 0, 0);
}

void Scene::update() {
	updateBackground();
	updateAnimations();
	updateSprites();
}

void Scene::updateBackground() {
	auto& sprite = _registry.get<SpriteComponent>(_background);
	SDL2::Rect src = { 0, 0, 1600, 1296 };
	SDL2::Rect dest = { 0, 0, 800, 648 };
	SDL2::blit(_renderer, sprite.tex, src, dest);
}

void Scene::updateAnimations() {
	auto& view = _registry.view<AnimationComponent, SpriteComponent, TransformComponent>();
	for (auto entity : view) {
		auto& [animation, sprite, transform] = view.get(entity);
		int spritePos = animation.current / animation.period;
		sprite.x = spritePos * transform.width;

		++animation.current;
		if (animation.current == animation.wavelength)
			animation.current = 0;
	}
}

void Scene::updateSprites() {
	auto& view = _registry.view<SpriteComponent, TransformComponent>();
	for (auto entity : view) {
		auto& [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
		SDL2::Rect dest = { transform.x, transform.y, transform.width, transform.height };
		SDL2::Rect src = { sprite.x, sprite.y, transform.width, transform.height };
		SDL2::blit(_renderer, sprite.tex, src, dest);
	}
}