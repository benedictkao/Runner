#pragma once

#include "PlayerController.h"
#include "SDL.h"

class KeyboardController {
private:
	PlayerController& _pControl;

public:
	KeyboardController(PlayerController&);

	void handleKeyDown(SDL2::Keycode);
	void handleKeyUp(SDL2::Keycode);
};