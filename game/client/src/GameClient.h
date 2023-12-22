#pragma once

#include "sdl/SDL.h"

class GameClient 
{
private:
	SDL2::Window _window;
	SDL2::Renderer _renderer;
	bool _running;

public:
	GameClient();

	int run();

	Uint64 calculateSleepTime(Uint64);
};