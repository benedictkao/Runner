#pragma once

#include "../PlayerState.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"

class InputManager {
public:
	virtual	bool readInput() = 0;
};

class MKInputManager : public InputManager {
private:
	KeyboardHandler _kHandler;
	MouseHandler _mHandler;

public:
	bool readInput();

	MKInputManager(PlayerState&);
};