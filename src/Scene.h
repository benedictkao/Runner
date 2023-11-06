#pragma once

#include "SDL.h"
#include "PlayerState.h"
#include "TextureRepo.h"
#include "../vendor/entt.hpp"

class Scene {
private:
	SDL2::Renderer _renderer;
	PlayerState& _pControl;
	TextureRepo& _texRepo;
	entt::registry _registry;
	entt::entity _player;
	entt::entity _background;

public:
	Scene(SDL2::Renderer, PlayerState&, TextureRepo&);

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