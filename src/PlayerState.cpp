#include "PlayerState.h"
#include "math/Math.h"

#include <iostream>

constexpr int LEFT = -1;
constexpr int RIGHT = 1;

void PlayerState::moveLeft() {
	_inputMovement = LEFT;
}

PlayerState::PlayerState() : _inputMovement(0), _isJumping(false), _onGround(false) {}

void PlayerState::moveRight() {
	_inputMovement = RIGHT;
}

void PlayerState::stopMoveLeft() {
	math::coercePositive(_inputMovement);
}

void PlayerState::stopMoveRight() {
	math::coerceNegative(_inputMovement);
}

void PlayerState::setJumping(bool isJumping) {
	_isJumping = isJumping;
}

int PlayerState::getCurrentMovement() const {
	return _inputMovement;
}

bool PlayerState::isJumping() const {
	return _isJumping;
}

bool PlayerState::isOnGround() const {
	return _onGround;
}

void PlayerState::setOnGround(bool onGround) {
	_onGround = onGround;
}