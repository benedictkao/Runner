#include "MouseHandler.h"

#include "../util/Logger.h"

MouseHandler::MouseHandler() : _cursorPos({ 0, 0 }) {}

void MouseHandler::handleMouseUp(SDL2::MouseEvent event, InputState& state) {
	if (event.button != SDL_BUTTON_LEFT) {
		debug::log("Unregistered mouse button up");
	}
	else {
		debug::log("Register left click at: (%d, %d)", _cursorPos.x, _cursorPos.y);
		state.selectPos = _cursorPos;
	}
}

void MouseHandler::handleMouseDown(SDL2::MouseEvent event, InputState& state) {
	_cursorPos = SDL2::getMouseCoordinates();
	debug::log("Left mouse button down at: (%d, %d)", _cursorPos.x, _cursorPos.y);
}