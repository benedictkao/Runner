#include "Components.h"
#include <iostream>

TagComponent::TagComponent(const char* tag) : tag(tag) {}

SpriteComponent::SpriteComponent(SDL2::Texture tex, float w, float h)
	: tex(tex), size({ w, h }), pos({ 0, 0 }), padding({ 0, 0, 0, 0 }), flipHorizontal(false) {}

AnimationComponent::AnimationComponent(int period, int wavelength) : period(period), wavelength(wavelength), current(0) {}

VelocityComponent::VelocityComponent() : dir({ 0, 0 }) {}

Vector2D Vector2D::operator+(const Vector2D& v) const {
	return { x + v.x, y + v.y };
}

Vector2D Vector2D::operator-(const Vector2D& v) const {
	return { x - v.x, y - v.y };
}

Vector2D Vector2D::operator*(const Vector2D& v) const {
	return { x * v.x, y * v.y };
}

Vector2D Vector2D::operator/(const Vector2D& v) const {
	return { x / v.x, y / v.y };
}

Vector2Df Vector2Df::operator+(const Vector2Df& v) const {
	return { x + v.x, y + v.y };
}

Vector2Df Vector2Df::operator-(const Vector2Df& v) const {
	return { x - v.x, y - v.y };
}

Vector2Df Vector2Df::operator*(const Vector2Df& v) const {
	return { x * v.x, y * v.y };
}

Vector2Df Vector2Df::operator/(const Vector2Df& v) const {
	return { x == 0 ? x : x / v.x, y == 0 ? y : y / v.y };
}

Vector2Df Vector2Df::operator*(float f) const {
	return { x * f, y * f };
}

Vector2Df Vector2Df::operator/(float f) const {
	return { x == 0 ? x : x / f, y == 0 ? y : y / f };
}

Vector2Df& Vector2Df::operator+=(const Vector2Df& v) {
	x += v.x;
	y += v.y;
	return *this;
}
