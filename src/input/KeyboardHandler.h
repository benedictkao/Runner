#pragma once

#include "../PlayerState.h"
#include "../SDL.h"

class KeyboardHandler {
private:
	PlayerState& _pControl;

public:
	KeyboardHandler(PlayerState&);

	void handleKeyDown(SDL2::Keycode);
	void handleKeyUp(SDL2::Keycode);
};