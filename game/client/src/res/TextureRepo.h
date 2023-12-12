#pragma once

#include "sdl/SDL.h"
#include "res/ResIds.h"
#include <unordered_map>

class TextureRepo {
private:
	SDL2::Renderer _renderer;
	std::unordered_map<int, SDL2::Texture> _texMap;

public:
	TextureRepo(SDL2::Renderer);

	SDL2::Texture loadTexture(int id);
	void clear();
};