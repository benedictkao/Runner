#pragma once

#include <SDL.h>

namespace SDL2 {
	typedef SDL_Window* Window;
	typedef SDL_Renderer* Renderer;
	typedef SDL_Event Event;
	typedef SDL_Texture* Texture;
	typedef SDL_Rect Rect;
	typedef SDL_Keycode Keycode;

	constexpr auto INIT_SUCCESS{ 0 };

	Uint32 init();

	Window createWindow(const char*);

	Renderer createRenderer(Window);

	Uint32 pollEvent(Event*);

	Uint64 elapsedTimeInMillis();

	Texture loadTexture(Renderer, const char*);

	void destroyTexture(Texture);

	bool hasIntersect(const Rect&, const Rect&);

	bool hasIntersect(const Rect&, const Rect&, Rect* result);

	void blit(Renderer, Texture, const Rect& dest);

	void blit(Renderer, Texture, const Rect& src, const Rect& dest);

	void blit(Renderer, Texture, const Rect& src, const Rect& dest, bool flipHorizontal);

	void renderAll(Renderer);

	void delay(Uint32);

	void close(Window, Renderer);
}