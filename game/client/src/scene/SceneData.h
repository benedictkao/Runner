#pragma once

#include <vector>
#include "math/Geometry.h"

struct Object
{
    Rect2Df transform;
    unsigned int texId;
};

struct PlayerInfo
{
    Vector2Df spawnLocation;
};

struct SceneData
{
    PlayerInfo playerInfo;
    std::vector<Object> objects;
};