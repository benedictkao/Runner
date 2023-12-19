#pragma once

#include <common/Geometry.h>

enum InputAction : unsigned int {
	NONE = 0,
	JUMP
};

class InputState {
public:
	common::Vector2D arrows;
	common::Vector2D selectPos;

	InputState();

private:
	unsigned int _actions;

	void setAction(InputAction);
	void unsetAction(InputAction);
	bool hasAction(InputAction);
};