#pragma once

#include "SDL.h"
#include <entt/entt.hpp>
#include "Systems/SpriteSystem.h"

class Game
{
private:
	SDL2::Window _window;
	SDL2::Renderer _renderer;
	bool _running;
	entt::registry registry;
	SpriteSystem sprite_system;

public:
	struct position
	{
		float x, y;
	};

	struct velocity
	{
		float dx, dy;
	};

	Game();

	int run();

	void handleEvents();

	Uint64 calculateSleepTime(Uint64);
};