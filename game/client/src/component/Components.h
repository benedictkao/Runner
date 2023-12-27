#pragma once

#include <common/Geometry.h>
#include <entt/entt.hpp>

#include "sdl/SDL.h"

struct TagComponent {
	const char* tag;

	TagComponent(const char*);
};

struct TransformComponent {
	common::Rect2Df rect;
};

struct SpriteComponent {
	SDL2::Texture tex;
	common::Rect2Df srcRect; // srcRect in the sprite pack
	common::Vector2Df offset; // offset from hit box
	float scale;
	bool flipHorizontal;

	SpriteComponent(common::Vector2Df size);
	SpriteComponent(SDL2::Texture, common::Vector2Df size);
	SpriteComponent(SDL2::Texture, common::Vector2Df size, float scale);
};

struct AnimationComponent {
	int period;
	int wavelength;
	int current;

	AnimationComponent(int period, int wavelength);
};

struct VelocityComponent {
	common::Vector2Df vector;

	VelocityComponent();
};

struct GravityComponent {};

struct CollisionComponent {};