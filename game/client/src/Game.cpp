#include "Game.h"

#include <thread>

#include <logging/Logger.h>

#include "input/InputManager.h"
#include "math/Math.h"
#include "network/ConnectionManager.h"
#include "player/PlayerManager.h"
#include "scene/Scene.h"
#include "scene/SceneLoader.h"
#include "sdl/SDL.h"

static constexpr auto TARGET_FPS{ 60 };
static constexpr auto MILLIS_PER_FRAME{ 1000 / TARGET_FPS };
enum GAME_INIT_ERROR { SUBSYSTEM = 1, WINDOW, RENDERER };

Game::Game() : _window(nullptr), _renderer(nullptr), _running(false) {}

int Game::run() {
	const int initResult = SDL2::init();
	if (initResult != SDL2::INIT_SUCCESS)
		return GAME_INIT_ERROR::SUBSYSTEM;

	_window = SDL2::createWindow("Runner");
	if (!_window)
		return GAME_INIT_ERROR::WINDOW;

	_renderer = SDL2::createRenderer(_window);
	if (!_renderer)
		return GAME_INIT_ERROR::RENDERER;

	debug::log("[SDL] Init success!");

	ConnectionManager connMgr;
	std::thread net_thread([&]() {
		connMgr.connect();
	});
	_running = true;

	TextureRepo texRepo(_renderer);
	MKInputManager inputManager;
	PlayerManager pManager(inputManager);
	Scene scene(_renderer, pManager, texRepo, connMgr);
	SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	SDL_SetCursor(cursor);
	
	scene.init(SceneLoader::load(0));

	while (_running) {
		// main game loop

		Uint64 frameStart = SDL2::elapsedTimeInMillis();

		connMgr.processReceivedMessages();

		SDL2::prepareScene(_renderer);
		_running = inputManager.readInput();

		// calculations
		scene.updateLogic();

		// ui updates
		scene.updateTextures();

		// actual render
		SDL2::presentScene(_renderer);

		Uint64 sleepTime = calculateSleepTime(frameStart);
		SDL2::delay(static_cast<Uint32>(sleepTime));
	}

	connMgr.close();
	texRepo.clear();
	net_thread.join();
	SDL2::close(_window, _renderer);
	return 0;
}

Uint64 Game::calculateSleepTime(Uint64 frameStart) {
	Uint64 actualFrameTime = frameStart - SDL2::elapsedTimeInMillis();
	Uint64 sleepTime = std::max<Uint64>(MILLIS_PER_FRAME - actualFrameTime, 0);
	return sleepTime;
}