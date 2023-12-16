#pragma once

#include <vector>
#include "math/Geometry.h"

struct SpriteInfo
{
    unsigned int texId;
    Vector2Df size;
};

struct WallInfo
{
    bool left;
    bool right;
    bool up;
    bool down;

    constexpr static WallInfo all() 
    {
        return { true, true, true, true };
    }

    constexpr static WallInfo getDefault()
    {
        return { true, true, true, false };
    }
};

struct MapInfo
{
    SpriteInfo bg;
    Vector2Df size;
    WallInfo walls;
};

struct SceneObject
{
    Rect2Df transform;
    SpriteInfo sprite;
    bool hasCollision;
};

struct PlayerInfo
{
    Rect2Df transform;
};

struct SceneInfo
{
    MapInfo mapInfo;
    std::vector<SceneObject> objects;
    PlayerInfo playerInfo;
};