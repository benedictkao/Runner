#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler(PlayerState& pControl) : _pControl(pControl) {}

void KeyboardHandler::handleKeyDown(SDL2::Keycode code) {
	switch (code) {
	case SDLK_LEFT:
		_pControl.moveLeft();
		break;
	case SDLK_RIGHT:
		_pControl.moveRight();
		break;
	case SDLK_SPACE:
		_pControl.setJumping(true);
		break;

		// not yet supported
	case SDLK_UP:
	case SDLK_DOWN:
		break;
	default:
		break;
	}
}

void KeyboardHandler::handleKeyUp(SDL2::Keycode code) {
	switch (code) {
	case SDLK_LEFT:
		_pControl.stopMoveLeft();
		break;
	case SDLK_RIGHT:
		_pControl.stopMoveRight();
		break;
	case SDLK_SPACE:
		_pControl.setJumping(false);
		break;

		// not yet supported
	case SDLK_UP:
	case SDLK_DOWN:
		break;
	default:
		break;
	}
}
