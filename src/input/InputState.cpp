#include "InputState.h"

void InputState::setAction(InputAction action) {
	_actions |= static_cast<unsigned int>(action);
}

void InputState::unsetAction(InputAction action) {
	_actions &= ~static_cast<unsigned int>(action);
}

InputState::InputState() : arrows({ 0, 0 }), selectPos({ -1, -1 }), _actions(InputAction::NONE) {}

bool InputState::hasAction(InputAction action) {
	return _actions & static_cast<unsigned int>(action);
}