#pragma once

#include <vector>

#include <common/Geometry.h>

struct SpriteInfo
{
    unsigned int id;
    common::Vector2Df size;
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
    common::Vector2Df size;
    WallInfo walls;
};

struct SceneObject
{
    common::Rect2Df transform;
    SpriteInfo sprite;
    bool hasCollision;
};

struct PlayerInfo
{
    common::Rect2Df transform;
};

struct SceneInfo
{
    MapInfo mapInfo;
    std::vector<SceneObject> objects;
    PlayerInfo playerInfo;
};