#pragma once

#include <common/Geometry.h>

namespace TextureIds
{
	namespace Background
	{
		constexpr auto START { 0 };

		constexpr auto DEFAULT { START + 0 };

		constexpr auto END { DEFAULT };
	}

	namespace Sprite
	{
		constexpr auto START { Background::END + 1 };

		constexpr auto PINK_MONSTER_IDLE { START + 0 };
		constexpr auto PINK_MONSTER_RUN { START + 1 };
		constexpr auto PINK_MONSTER_JUMP { START + 2 };
		
		constexpr auto OWLET_MONSTER_IDLE { START + 3 };
		constexpr auto OWLET_MONSTER_RUN { START + 4 };
		constexpr auto OWLET_MONSTER_JUMP { START + 5 };
		
		constexpr auto DUDE_MONSTER_IDLE { START + 6 };
		constexpr auto DUDE_MONSTER_RUN { START + 7 };
		constexpr auto DUDE_MONSTER_JUMP { START + 8 };
		
		constexpr auto END { DUDE_MONSTER_JUMP };
	}

	namespace Tile
	{
		constexpr auto START { Sprite::END + 1 };

		constexpr auto BLUE { START + 0 };
		constexpr auto RED_STRIPED { START + 1 };

		constexpr auto END { RED_STRIPED };
	}

	namespace Icon
	{
		constexpr auto START { Tile::END + 1 };

		constexpr auto CONNECTED { START + 0 };
		constexpr auto DISCONNECTED { START + 1 };

		constexpr auto END { DISCONNECTED };
	}

	namespace Object
	{
		constexpr auto START { Icon::END + 1 };

		constexpr auto HAMMER { START + 0 };

		constexpr auto END { HAMMER };
	}

	constexpr const char* PATHS[] = 
	{
		/*
		* Backgrounds
		*/
		"../../../../../game/client/res/texture/background/Background.png",

		/*
		* Sprites
		*/
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Idle_4.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Run_6.png",
		"../../../../../game/client/res/texture/sprite/Pink_Monster_Jump_8.png",

		"../../../../../game/client/res/texture/sprite/Owlet_Monster_Idle_4.png",
		"../../../../../game/client/res/texture/sprite/Owlet_Monster_Run_6.png",
		"../../../../../game/client/res/texture/sprite/Owlet_Monster_Jump_8.png",

		"../../../../../game/client/res/texture/sprite/Dude_Monster_Idle_4.png",
		"../../../../../game/client/res/texture/sprite/Dude_Monster_Run_6.png",
		"../../../../../game/client/res/texture/sprite/Dude_Monster_Jump_8.png",
		
		/*
		* Tiles
		*/
		"../../../../../game/client/res/texture/tile/IndustrialTile_14.png",
		"../../../../../game/client/res/texture/tile/IndustrialTile_09.png",

		/*
		* Icons
		*/
		"../../../../../game/client/res/texture/icon/green_circle.png",
		"../../../../../game/client/res/texture/icon/red_circle.png",

		/*
		* Objects
		*/
		"../../../../../game/client/res/texture/objects/Hammer.png"
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
	constexpr unsigned int OWLET_MONSTER { 1 };
	constexpr unsigned int DUDE_MONSTER { 2 };

	constexpr SpritePack PACKS[] = {
		{ TextureIds::Sprite::PINK_MONSTER_IDLE, TextureIds::Sprite::PINK_MONSTER_RUN, TextureIds::Sprite::PINK_MONSTER_JUMP, { 32, 32 }, { 6, 4 } },
		{ TextureIds::Sprite::OWLET_MONSTER_IDLE, TextureIds::Sprite::OWLET_MONSTER_RUN, TextureIds::Sprite::OWLET_MONSTER_JUMP, { 32, 32 }, { 6, 4 } },
		{ TextureIds::Sprite::DUDE_MONSTER_IDLE, TextureIds::Sprite::DUDE_MONSTER_RUN, TextureIds::Sprite::DUDE_MONSTER_JUMP, { 32, 32 }, { 6, 4 } }
	};

	constexpr SpritePack getPack(unsigned int id)
	{
		return PACKS[id];
	}
}