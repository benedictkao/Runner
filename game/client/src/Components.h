#pragma once

#include "sdl/SDL.h"
#include "math/Geometry.h"

struct TagComponent {
	const char* tag;

	TagComponent(const char*);
};

struct TransformComponent {
	Rect2Df rect;
};

struct SpriteComponent {
	SDL2::Texture tex;
	Rect2Df srcRect; // srcRect in the sprite pack
	Vector2Df offset; // offset from hit box
	float scale;
	bool flipHorizontal;

	SpriteComponent(SDL2::Texture, float w, float h);
	SpriteComponent(SDL2::Texture, float w, float h, float scale);
};

struct AnimationComponent {
	int period;
	int wavelength;
	int current;

	AnimationComponent(int period, int wavelength);
};

struct VelocityComponent {
	Vector2Df vector;

	VelocityComponent();
};

struct GravityComponent {};

struct WallComponent {};