#pragma once

class PlayerController {
private:
	int _inputMovement;
	bool _isJumping;
	bool _onGround;

public:
	PlayerController();

	void moveLeft();
	void moveRight();
	void setJumping(bool);

	void stopMoveLeft();
	void stopMoveRight();

	int getCurrentMovement() const;
	bool isJumping() const;

	bool isOnGround() const;
	void setOnGround(bool);
};