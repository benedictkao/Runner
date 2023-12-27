#pragma once

#include <vector>

#include <common/Geometry.h>

struct SpriteInfo
{
    unsigned int id;
    common::Vector2Df size;
};

struct MapInfo
{
    SpriteInfo bg;
    common::Vector2Df size;
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