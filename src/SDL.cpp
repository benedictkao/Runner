#include <SDL.h>
#include <SDL_image.h>

static constexpr auto DEFAULT_WINDOW_WIDTH{ 800 };
static constexpr auto DEFAULT_WINDOW_HEIGHT{ 648 };

Uint32 SDL2::init() {
	return SDL_Init(SDL_INIT_VIDEO);
}

SDL2::Window SDL2::createWindow(const char* title) {
	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, false);
}

SDL2::Renderer SDL2::createRenderer(SDL2::Window window) {
	return SDL_CreateRenderer(window, -1, 0);
}

Uint32 SDL2::pollEvent(SDL2::Event* event) {
	return SDL_PollEvent(event);
}

Uint64 SDL2::elapsedTimeInMillis() {
	return SDL_GetTicks64();
}

SDL2::Texture SDL2::loadTexture(Renderer renderer, const char* path) {
	SDL_Texture* tex = IMG_LoadTexture(renderer, path);
	if (!tex)
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
			SDL_LOG_PRIORITY_INFO,
			"Failed to load %s. Error: %s",
			path,
			IMG_GetError());
	else
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
			SDL_LOG_PRIORITY_INFO,
			"Loaded texture %s",
			path);
	return tex;
}

void SDL2::blit(Renderer renderer, Texture tex, const Rect& dest) {
	SDL_RenderCopy(renderer, tex, NULL, &dest);
}

void SDL2::blit(Renderer renderer, Texture tex, const Rect& src, const Rect& dest) {
	SDL_RenderCopy(renderer, tex, &src, &dest);
}

void SDL2::renderAll(Renderer renderer) {
	SDL_RenderPresent(renderer);
}

void SDL2::delay(Uint32 timeInMillis) {
	SDL_Delay(timeInMillis);
}

void SDL2::close(SDL2::Window window) {
	SDL_DestroyWindow(window);
	SDL_Quit();
}