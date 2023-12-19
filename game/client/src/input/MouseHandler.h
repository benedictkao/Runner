#pragma once

#include <common/Geometry.h>

#include "sdl/SDL.h"
#include "InputState.h"

class MouseHandler {
private:
	common::Vector2D _cursorPos;

public:
	MouseHandler();

	void handleMouseUp(SDL2::MouseEvent, InputState&);
	void handleMouseDown(SDL2::MouseEvent, InputState&);
};