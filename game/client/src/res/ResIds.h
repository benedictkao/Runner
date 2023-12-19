#pragma once

#include <common/Geometry.h>

namespace TextureIds
{
	constexpr auto BACKGROUND = 0;
	constexpr auto PLAYER_IDLE = 1;
	constexpr auto PLAYER_RUN = 2;
	constexpr auto PLAYER_JUMP = 3;
	constexpr auto TILE = 4;
	constexpr auto RED_TILE = 5;
	constexpr auto ICON_CONNECTED = 6;
	constexpr auto ICON_DISCONNECTED = 7;

	constexpr const char* PATHS[] = 
	{
		"../../../../../game/client/res/texture/background/Background.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Idle_4.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Run_6.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Jump_8.png",
		"../../../../../game/client/res/texture/tile/IndustrialTile_14.png",
		"../../../../../game/client/res/texture/tile/IndustrialTile_09.png",
		"../../../../../game/client/res/texture/icon/green_circle.png",
		"../../../../../game/client/res/texture/icon/red_circle.png"
	};

	constexpr const char* getPath(unsigned int id) 
	{
		return PATHS[id];
	}
}

namespace SpriteIds
{
	struct SpritePack
	{
		unsigned int idle;
		unsigned int run;
		unsigned int jump;
		common::Vector2Df size;
		common::Vector2Df offset;
	};

	constexpr unsigned int PINK_MONSTER { 0 };

	constexpr SpritePack PACKS[] = {
		{ TextureIds::PLAYER_IDLE, TextureIds::PLAYER_RUN, TextureIds::PLAYER_JUMP, { 32, 32 }, { 6, 4 } }
	};

	constexpr SpritePack getPack(unsigned int id)
	{
		return PACKS[id];
	}
}