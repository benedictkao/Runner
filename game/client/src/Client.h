#pragma once

#include "sdl/SDL.h"

class Client 
{
private:
	SDL2::Window _window;
	SDL2::Renderer _renderer;
	bool _running;

public:
	Client();

	int run();

	Uint64 calculateSleepTime(Uint64);
};