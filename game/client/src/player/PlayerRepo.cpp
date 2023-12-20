#include "PlayerRepo.h"

#include <logging/Logger.h>

void PlayerRepo::onPlayerJoin(const common::messages::JoinDetails& details)
{
	_localPlayerId = details.playerId;
	debug::log("[PlayerRepo] Player joined game! Assigned id %d", _localPlayerId);
}

int PlayerRepo::getPlayerId() const
{
	return _localPlayerId;
}

void PlayerRepo::onPlayerUpdate(const common::messages::PlayerUpdate& update)
{
	auto id = update.playerId;
	_remoteData[id] = { update.spriteId, update.transform, update.speed, true };
}

PlayerRepo::PlayerRepo(): 
	_localPlayerId(Player::DEFAULT_ID), 
	_remoteData(common::MAX_PLAYERS_PER_GAME, { -1, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }) 
{}

const std::vector<PlayerData> PlayerRepo::getRemoteData() const
{
	return _remoteData;
}