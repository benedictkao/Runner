#pragma once

#include "math/Geometry.h"

enum InputAction : unsigned int {
	NONE = 0,
	JUMP
};

class InputState {
public:
	Vector2D arrows;
	Vector2D selectPos;

	InputState();

private:
	unsigned int _actions;

	void setAction(InputAction);
	void unsetAction(InputAction);
	bool hasAction(InputAction);
};