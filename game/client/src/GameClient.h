#pragma once

#include <network/Common.h>
#include <stdint.h>

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

	uint64_t calculateSleepTime(network::TimeUnit);
};