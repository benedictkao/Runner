#include "KeyboardHandler.h"

#include "../math/Math.h"

void KeyboardHandler::handleKeyDown(SDL2::Keycode code, InputState& state) {
	switch (code) {
	case SDLK_LEFT:
		state.arrows.x = -1;
		break;
	case SDLK_RIGHT:
		state.arrows.x = 1;
		break;
	case SDLK_SPACE:
		state.arrows.y = 1;
		break;

		// not yet supported
	case SDLK_UP:
	case SDLK_DOWN:
		break;
	default:
		break;
	}
}

void KeyboardHandler::handleKeyUp(SDL2::Keycode code, InputState& state) {
	switch (code) {
	case SDLK_LEFT:
		math::coerceAtLeast(state.arrows.x, 0);
		break;
	case SDLK_RIGHT:
		math::coerceAtMost(state.arrows.x, 0);
		break;
	case SDLK_SPACE:
		state.arrows.y = 0;
		break;

		// not yet supported
	case SDLK_UP:
	case SDLK_DOWN:
		break;
	default:
		break;
	}
}
