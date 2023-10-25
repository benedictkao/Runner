#pragma once

#include "SDL.h"

struct TagComponent {
	const char* tag;
};

struct TransformComponent {
	int x;
	int y;
	int width;
	int height;
};

struct SpriteComponent {
	SDL2::Texture tex;
	int x;
	int y;
};

struct AnimationComponent {
	int period;
	int current;
	int wavelength;
};