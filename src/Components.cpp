#include "Components.h"
#include <iostream>

TagComponent::TagComponent(const char* tag) : tag(tag) {}

SpriteComponent::SpriteComponent(SDL2::Texture tex, float w, float h)
	: tex(tex), srcRect({ 0, 0, w, h }), offset({ 0, 0 }), flipHorizontal(false) {}

AnimationComponent::AnimationComponent(int period, int wavelength) : period(period), wavelength(wavelength), current(0) {}

VelocityComponent::VelocityComponent() : vector({ 0, 0 }) {}