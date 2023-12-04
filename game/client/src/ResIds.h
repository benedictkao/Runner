#pragma once

namespace TextureIds {
	constexpr auto BACKGROUND = 0;
	constexpr auto PLAYER_IDLE = 1;
	constexpr auto PLAYER_RUN = 2;
	constexpr auto PLAYER_JUMP = 3;
	constexpr auto TILE = 4;
	constexpr auto RED_TILE = 5;

	constexpr const char* PATHS[] = {
		"../../../../../game/client/res/texture/background/Background.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Idle_4.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Run_6.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Jump_8.png",
		"../../../../../game/client/res/texture/tile/IndustrialTile_14.png",
		"../../../../../game/client/res/texture/tile/IndustrialTile_09.png"
	};

	constexpr const char* getPath(unsigned int id) {
		return PATHS[id];
	}
}