#pragma once

#include <vector>

#include <common/Geometry.h>
#include <common/Network.h>

namespace Player
{
	constexpr int DEFAULT_ID { -1 };
}

struct PlayerData
{
	int spriteId;
	common::Rect2Df transform;
	common::Vector2Df speed;
	bool connected;
};

class PlayerRepo
{
public:
	PlayerRepo();

public:
	void onPlayerJoin(const common::messages::JoinDetails&);
	int getPlayerId() const;
	void onPlayerUpdate(const common::messages::PlayerUpdate&);
	const std::vector<PlayerData> getRemoteData() const;

private:
	int _localPlayerId;
	std::vector<PlayerData> _remoteData;
};