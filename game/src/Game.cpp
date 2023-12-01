#include "Game.h"

#include "SDL.h"
#include "math/Math.h"
#include "Scene.h"
#include "PlayerManager.h"
#include "util/Logger.h"
#include "input/InputManager.h"

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

	debug::log("Init success!");

	_running = true;

	TextureRepo texRepo(_renderer);
	MKInputManager inputManager;
	PlayerManager pManager(inputManager);
	Scene scene(_renderer, pManager, texRepo);
	SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	SDL_SetCursor(cursor);
	scene.init();

	while (_running) {
		// main game loop

		Uint64 frameStart = SDL2::elapsedTimeInMillis();

		SDL2::prepareScene(_renderer);
		_running = inputManager.readInput();

		// calculations
		scene.update();

		SDL2::presentScene(_renderer);

		Uint64 sleepTime = calculateSleepTime(frameStart);
		SDL2::delay(sleepTime);
	}

	texRepo.clear();
	SDL2::close(_window, _renderer);
	return 0;
}

Uint64 Game::calculateSleepTime(Uint64 frameStart) {
	Uint64 actualFrameTime = frameStart - SDL2::elapsedTimeInMillis();
	Uint64 sleepTime = std::max<Uint64>(MILLIS_PER_FRAME - actualFrameTime, 0);
	return sleepTime;
}