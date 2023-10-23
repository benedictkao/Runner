#pragma once

#include "SDL.h"
#include <entt/entt.hpp>

class Game
{
private:
	SDL2::Window _window;
	SDL2::Renderer _renderer;
	bool _running;
	entt::registry registry;

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