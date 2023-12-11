#include "Components.h"
#include <iostream>

TagComponent::TagComponent(const char* tag) : tag(tag) {}

SpriteComponent::SpriteComponent(SDL2::Texture tex, Vector2Df size)
	: tex(tex), srcRect({ 0, 0, size.x, size.y }), offset({ 0, 0 }), scale(1), flipHorizontal(false) {}

SpriteComponent::SpriteComponent(SDL2::Texture tex, float w, float h, float scale)
	: tex(tex), srcRect({ 0, 0, w, h }), offset({ 0, 0 }), scale(scale), flipHorizontal(false) {}

AnimationComponent::AnimationComponent(int period, int wavelength) : period(period), wavelength(wavelength), current(0) {}

VelocityComponent::VelocityComponent() : vector({ 0, 0 }) {}