#include "Game.h"

#include <SDL.h>
#include "math/Math.h"
#include "PlayerController.h"
#include "Scene.h"
#include "util/Logger.h"

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

	PlayerController pControl;
	TextureRepo texRepo(_renderer);
	KeyboardController kControl(pControl);
	Scene scene(_renderer, pControl, texRepo);
	scene.init();

	while (_running) {
		// main game loop

		Uint64 frameStart = SDL2::elapsedTimeInMillis();

		SDL2::prepareScene(_renderer);
		handleEvents(kControl);

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

void Game::handleEvents(KeyboardController& kControl) {
	SDL2::Event event;
	while (SDL2::pollEvent(&event)) {
		switch (event.type) {	// add other events below here
		case SDL_QUIT:
			_running = false;
			break;
		case SDL_KEYDOWN:
			kControl.handleKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			kControl.handleKeyUp(event.key.keysym.sym);
			break;
		default:
			break;
		}
	}
}

Uint64 Game::calculateSleepTime(Uint64 frameStart) {
	Uint64 actualFrameTime = frameStart - SDL2::elapsedTimeInMillis();
	Uint64 sleepTime = std::max<Uint64>(MILLIS_PER_FRAME - actualFrameTime, 0);
	return sleepTime;
}