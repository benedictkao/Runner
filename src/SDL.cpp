#include <SDL.h>
#include <SDL_image.h>

#include "Constants.h"

Uint32 SDL2::init() {
	return SDL_Init(SDL_INIT_VIDEO);
}

SDL2::Window SDL2::createWindow(const char* title) {
	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT, false);
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

void SDL2::destroyTexture(SDL2::Texture tex) {
	SDL_DestroyTexture(tex);
}

bool SDL2::hasIntersect(const Rect& a, const Rect& b) {
	return static_cast<bool>(SDL_HasIntersection(&a, &b));
}

bool SDL2::hasIntersect(const Rect& a, const Rect& b, Rect* result) {
	return static_cast<bool>(SDL_IntersectRect(&a, &b, result));
}

void SDL2::blit(Renderer renderer, Texture tex, const Rect& dest) {
	SDL_RenderCopy(renderer, tex, NULL, &dest);
}

void SDL2::blit(Renderer renderer, Texture tex, const Rect& src, const Rect& dest) {
	SDL_RenderCopy(renderer, tex, &src, &dest);
}

void SDL2::blit(Renderer renderer, Texture tex, const Rect& src, const Rect& dest, bool flipHorizontal) {
	SDL_RendererFlip flipFlags = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	//int status = SDL_RenderCopyEx(renderer, tex, &src, &dest, 0, NULL, flipFlags);
	//if (status != 0)
	//	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
	//		SDL_LOG_PRIORITY_INFO,
	//		"Failed to render. Error: %s",
	//		SDL_GetError());
	SDL_RenderCopyEx(renderer, tex, &src, &dest, 0, NULL, flipFlags);
}

void SDL2::prepareScene(Renderer renderer) {
	SDL_RenderClear(renderer);	// clears scene in case to prevent images from old frames being shown by accident
}

void SDL2::presentScene(Renderer renderer) {
	SDL_RenderPresent(renderer);
}

void SDL2::delay(Uint32 timeInMillis) {
	SDL_Delay(timeInMillis);
}

void SDL2::close(SDL2::Window window, SDL2::Renderer renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
}