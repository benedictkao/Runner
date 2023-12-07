#pragma once

#include "sdl/SDL.h"
#include "math/Geometry.h"
#include "InputState.h"

class MouseHandler {
private:
	Vector2D _cursorPos;

public:
	MouseHandler();

	void handleMouseUp(SDL2::MouseEvent, InputState&);
	void handleMouseDown(SDL2::MouseEvent, InputState&);
};