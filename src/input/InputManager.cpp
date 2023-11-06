#include "InputManager.h"

#include "../SDL.h"

bool MKInputManager::readInput() {
	SDL2::Event event;
	while (SDL2::pollEvent(&event)) {
		switch (event.type) {

			// mouse actions
		case SDL_MOUSEBUTTONUP:
			_mHandler.handleMouseUp(event.button);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_mHandler.handleMouseDown(event.button);
			break;

			// keyboard actions
		case SDL_KEYDOWN:
			_kHandler.handleKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_kHandler.handleKeyUp(event.key.keysym.sym);
			break;

			// quit
		case SDL_QUIT:
			return false;
		default:
			break;
		}
	}
	return true;
}

MKInputManager::MKInputManager(PlayerState& playerState) : _kHandler(playerState), _mHandler(playerState) {}