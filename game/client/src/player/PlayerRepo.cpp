#include "PlayerRepo.h"

void PlayerRepo::setPlayerId(int id)
{
	_id = id;
}

int PlayerRepo::getPlayerId() const
{
	return _id;
}