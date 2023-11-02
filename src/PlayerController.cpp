#include "PlayerController.h"
#include "math/Math.h"

#include <iostream>

constexpr int LEFT = -1;
constexpr int RIGHT = 1;

void PlayerController::moveLeft() {
	_inputMovement = LEFT;
}

PlayerController::PlayerController() : _inputMovement(0), _isJumping(false), _onGround(false) {}

void PlayerController::moveRight() {
	_inputMovement = RIGHT;
}

void PlayerController::stopMoveLeft() {
	math::coercePositive(_inputMovement);
}

void PlayerController::stopMoveRight() {
	math::coerceNegative(_inputMovement);
}

void PlayerController::setJumping(bool isJumping) {
	_isJumping = isJumping;
}

int PlayerController::getCurrentMovement() const {
	return _inputMovement;
}

bool PlayerController::isJumping() const {
	return _isJumping;
}

bool PlayerController::isOnGround() const {
	return _onGround;
}

void PlayerController::setOnGround(bool onGround) {
	_onGround = onGround;
}
