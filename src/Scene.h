#pragma once

#include "SDL.h"
#include "../vendor/entt.hpp"

class Scene {
private:
	SDL2::Renderer _renderer;
	entt::registry _registry;
	entt::entity _player;
	entt::entity _background;

public:
	Scene(SDL2::Renderer);

	void init();
	void update();

private:

	void updateBackground();
	void updateAnimations();
	void updateSprites();
};