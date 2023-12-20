#pragma once

#include <stdint.h>

#include "Geometry.h"

namespace common
{
	static constexpr int PORT_NUMBER{ 7777 };
	static constexpr int PING_INTERVAL_MILLIS{ 2500 };

	namespace messages
	{
		enum class Type : uint32_t
		{
			PING, PLAYER_UPDATE, PLAYER_JOIN, PLAYER_RECONNECT
		};

		struct PlayerUpdate
		{
			int playerId;
			int spriteId;
			Rect2Df transform;
			Vector2Df speed;
		};

		struct JoinDetails
		{
			int playerId;
		};
	}
}