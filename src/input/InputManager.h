#pragma once

#include "../PlayerState.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "InputState.h"

class InputManager {
public:
	virtual	bool readInput() = 0;
	const InputState& getState() const;

protected:
	InputState _state;
};

class MKInputManager : public InputManager {
private:
	KeyboardHandler _kHandler;
	MouseHandler _mHandler;

public:
	bool readInput();
};