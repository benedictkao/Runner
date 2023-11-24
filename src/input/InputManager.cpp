#include "InputManager.h"

#include "../SDL.h"

bool MKInputManager::readInput() {
	SDL2::Event event;
	while (SDL2::pollEvent(&event)) {
		switch (event.type) {

			// mouse _actions
		case SDL_MOUSEBUTTONUP:
			_mHandler.handleMouseUp(event.button, _state);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_mHandler.handleMouseDown(event.button, _state);
			break;

			// keyboard _actions
		case SDL_KEYDOWN:
			_kHandler.handleKeyDown(event.key.keysym.sym, _state);
			break;
		case SDL_KEYUP:
			_kHandler.handleKeyUp(event.key.keysym.sym, _state);
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

const InputState& InputManager::getState() const {
	return _state;
}