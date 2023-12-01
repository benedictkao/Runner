#pragma once

#include "InputState.h"
#include "../SDL.h"

class KeyboardHandler {
public:
	void handleKeyDown(SDL2::Keycode, InputState&);
	void handleKeyUp(SDL2::Keycode, InputState&);
};