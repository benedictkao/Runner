#include "GameClient.h"

#include <thread>

#include <logging/Logger.h>

#include "Constants.h"

#include "connection/ConnectionManager.h"
#include "input/InputManager.h"
#include "math/Math.h"
#include "player/PlayerManager.h"
#include "player/PlayerRepo.h"
#include "scene/Scene.h"
#include "scene/SceneLoader.h"
#include "sdl/SDL.h"

//#define LOG_FRAME_TIME

enum GAME_INIT_ERROR { SUBSYSTEM = 1, WINDOW, RENDERER };

GameClient::GameClient() : _window(nullptr), _renderer(nullptr), _running(false) {}

int GameClient::run() {
	const int initResult = SDL2::init();
	if (initResult != SDL2::INIT_SUCCESS)
		return GAME_INIT_ERROR::SUBSYSTEM;

	_window = SDL2::createWindow("Runner");
	if (!_window)
		return GAME_INIT_ERROR::WINDOW;

	_renderer = SDL2::createRenderer(_window);
	if (!_renderer)
		return GAME_INIT_ERROR::RENDERER;

	debug::log("[GameClient] SDL init success!");

	TextureRepo texRepo(_renderer);
	MKInputManager inputManager;
	PlayerRepo pRepo;
	ConnectionManager connMgr(pRepo);
	PlayerManager pManager(inputManager, pRepo);
	std::thread net_thread([&]() {
		connMgr.connect();
	});
	_running = true;

	Scene scene(_renderer, pManager, texRepo, connMgr);
	SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	SDL_SetCursor(cursor);
	
	scene.init(SceneLoader::load(0));

	while (_running) {
		// main game loop

		auto frameStart = network::currentTime();

		connMgr.processReceivedMessages();

		SDL2::prepareScene(_renderer);
		_running = inputManager.readInput();

		// update game logic
		scene.update();

		// update ui
		scene.blit();

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

uint64_t GameClient::calculateSleepTime(network::TimeUnit frameStart)
{
	auto actualFrameTime = network::currentTime() - frameStart;
	auto actualFrameTimeMillis = network::getMillis(actualFrameTime);
	auto sleepTime = std::max<long long>(constants::MILLIS_PER_FRAME - actualFrameTimeMillis, 0);
	#ifdef LOG_FRAME_TIME
	std::cout << network::getMicros(actualFrameTime) << "ms" << std::endl;
	#endif
	return static_cast<uint64_t>(sleepTime);
}