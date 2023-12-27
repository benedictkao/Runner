#include "SceneLoader.h"

#include "Constants.h"
#include "res/ResIds.h"

constexpr auto TILE_SIZE{ 32.0f };

SceneInfo SceneLoader::load(int level)
{
	// TODO: can load data from file based on level. For now, just hardcode

	SceneInfo data;
	data.mapInfo.bg = { TextureIds::Background::DEFAULT, 2304, 1296 };
	data.mapInfo.size = { constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT };
	data.mapInfo.walls = WallInfo::getDefault();

	data.objects.reserve(60);
	for (int i = 0; i < 30; ++i) {
		int rem = i % 3;
		bool topRow = i > 26;
		unsigned int texId = topRow ? TextureIds::Tile::RED_STRIPED : TextureIds::Tile::BLUE;
		common::Rect2Df transformL = {
			TILE_SIZE * rem,
			data.mapInfo.size.y - TILE_SIZE * (i / 3 + 1),
			TILE_SIZE,
			TILE_SIZE
		};
		common::Rect2Df transformR = {
			data.mapInfo.size.x - TILE_SIZE * (rem + 1),
			data.mapInfo.size.y - TILE_SIZE * (i / 3 + 1),
			TILE_SIZE,
			TILE_SIZE
		};
		SpriteInfo spriteInfo = { texId, 32, 32 };
		bool hasCollision = rem == 2 || topRow;

		data.objects.push_back({ transformL, spriteInfo, hasCollision });
		data.objects.push_back({ transformR, spriteInfo, hasCollision });
	}
	data.playerInfo.transform = { 50.0f, 200.0f, 40.0f, 56.0f };
	return data;
}
