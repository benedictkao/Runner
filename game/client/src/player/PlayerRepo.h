#pragma once

namespace Player
{
	constexpr int DEFAULT_ID { -1 };
}

class PlayerRepo
{
public:
	void setPlayerId(int id);
	int getPlayerId() const;

private:
	int _id = Player::DEFAULT_ID;
};