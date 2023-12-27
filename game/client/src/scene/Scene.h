#pragma once

#include <entt/entt.hpp>

#include "connection/ConnectionManager.h"
#include "player/PlayerManager.h"
#include "res/TextureRepo.h"
#include "sdl/SDL.h"

#include "SceneInfo.h"

class Scene {

public:
	Scene(SDL2::Renderer, PlayerManager&, TextureRepo&, ConnectionManager&);

	void init(const SceneInfo& data);
	void update();
	void blit();

private:
	void updateTransforms();
	void updateVelocities();
	void updateAnimations();

	void blitBackground();
	void blitSprites();
	void blitOverlay();

private:
	SDL2::Renderer _renderer;
	PlayerManager& _pManager;
	TextureRepo& _texRepo;
	ConnectionManager& _connMgr;
	entt::registry _registry;
	entt::entity _background;
};