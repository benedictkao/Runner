#pragma once

#include "SDL.h"

struct TagComponent {
	const char* tag;

	TagComponent(const char*);
};

struct Vector2D {
	int x;
	int y;

	Vector2D operator+(const Vector2D& v) const;
	Vector2D operator-(const Vector2D& v) const;
	Vector2D operator*(const Vector2D& v) const;
	Vector2D operator/(const Vector2D& v) const;
};

struct Vector2Df {
	float x;
	float y;

	Vector2Df operator+(const Vector2Df& v) const;
	Vector2Df operator-(const Vector2Df& v) const;
	Vector2Df operator*(const Vector2Df& v) const;
	Vector2Df operator/(const Vector2Df& v) const;

	Vector2Df operator*(float f) const;
	Vector2Df operator/(float f) const;

	Vector2Df& operator+=(const Vector2Df& v);
};

struct TransformComponent {
	Vector2Df pos;
	Vector2Df size;
};

struct Padding {
	int left;
	int right;
	int top;
	int bottom;
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
	Vector2Df dir;

	VelocityComponent();
};

struct GravityComponent {};

struct WallComponent {};