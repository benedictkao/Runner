#pragma once

#include "SDL.h"
#include "PlayerManager.h"
#include "TextureRepo.h"
#include "../vendor/entt.hpp"

class Scene {
private:
	SDL2::Renderer _renderer;
	PlayerManager& _pManager;
	TextureRepo& _texRepo;
	entt::registry _registry;
	entt::entity _player;
	entt::entity _background;

public:
	Scene(SDL2::Renderer, PlayerManager&, TextureRepo&);

	void init();
	void update();

private:

	void updateBackground();
	void updatePlayer();
	void updateSprites();
	void updateTransforms();
	void updateVelocities();
	void updateCollisions();
	void updateAnimations();
};