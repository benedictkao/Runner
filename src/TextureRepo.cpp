#include "TextureRepo.h"

TextureRepo::TextureRepo(SDL2::Renderer renderer) : _renderer(renderer) {}

SDL2::Texture TextureRepo::loadTexture(int id) {
	if (_texMap.count(id) > 0) {
		return _texMap[id];
	}
	else {
		SDL2::Texture tex = SDL2::loadTexture(_renderer, TextureIds::getPath(id));
		_texMap[id] = tex;
		return tex;
	}
}

void TextureRepo::clear() {
	for (auto& it : _texMap) {
		SDL2::destroyTexture(it.second);
	}
	_texMap.clear();
}