#pragma once

#include "SDL.h"
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
	Vector2Df size;
	Vector2Df pos;
	Padding padding;
	bool flipHorizontal;

	SpriteComponent(SDL2::Texture, float w, float h);
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