#pragma once

#include "sdl/SDL.h"

class Game {
private:
	SDL2::Window _window;
	SDL2::Renderer _renderer;
	bool _running;

public:
	Game();

	int run();

	Uint64 calculateSleepTime(Uint64);
};