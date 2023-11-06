#pragma once

#include "../PlayerState.h"
#include "../SDL.h"
#include "../math/Geometry.h"

class MouseHandler {
private:
	PlayerState& _pState;
	Vector2D _cursorPos;

public:
	MouseHandler(PlayerState&);

	void handleMouseUp(SDL2::MouseEvent);
	void handleMouseDown(SDL2::MouseEvent);
};