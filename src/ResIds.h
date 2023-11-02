#pragma once

namespace TextureIds {
	constexpr auto BACKGROUND = 0;
	constexpr auto PLAYER_IDLE = 1;
	constexpr auto PLAYER_RUN = 2;
	constexpr auto PLAYER_JUMP = 3;
	constexpr auto TILE = 4;

	constexpr const char* PATHS[] = {
		"../../../res/texture/background/4.png",
		"../../../res/texture/sprite/Idle.png",
		"../../../res/texture/sprite/Run.png",
		"../../../res/texture/sprite/Jump.png",
		"../../../res/texture/tile/Tile_03.png"
	};

	constexpr const char* getPath(unsigned int id) {
		return PATHS[id];
	}
}