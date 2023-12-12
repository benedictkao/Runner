#pragma once

#include <entt/entt.hpp>
#include <network/Client.h>

#include "player/PlayerManager.h"
#include "res/TextureRepo.h"
#include "sdl/SDL.h"

#include "SceneInfo.h"

class Scene {
private:
	SDL2::Renderer _renderer;
	PlayerManager& _pManager;
	TextureRepo& _texRepo;
	network::Client& _client;	// TODO: should probably have a wrapper around this!
	entt::registry _registry;
	entt::entity _player;
	entt::entity _background;

public:
	Scene(SDL2::Renderer, PlayerManager&, TextureRepo&, network::Client&);

	void init(SceneInfo data);
	void update();
	void render();

private:

	void updateBackground();
	void updatePlayer();
	void updateSprites();
	void updateTransforms();
	void updateVelocities();
	void updateCollisions();
	void updateAnimations();
	void updateOverlay();
};