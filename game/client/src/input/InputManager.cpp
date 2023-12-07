#include "InputManager.h"

#include "logging/Logger.h"
#include "../SDL.h"

bool MKInputManager::readInput() {
	SDL2::Event event;
	while (SDL2::pollEvent(&event)) {
		switch (event.type) {

			// mouse _actions
		case SDL_MOUSEBUTTONUP:
			_mHandler.handleMouseUp(event.button, _state);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_mHandler.handleMouseDown(event.button, _state);
			break;

			// keyboard _actions
		case SDL_KEYDOWN:
			_kHandler.handleKeyDown(event.key.keysym.sym, _state);
			break;
		case SDL_KEYUP:
			_kHandler.handleKeyUp(event.key.keysym.sym, _state);
			break;
		case SDL_WINDOWEVENT:
            handleWindowEvent(event.window);
			break;

			// quit
		case SDL_QUIT:
			return false;
		default:
			break;
		}
	}
	return true;
}

void MKInputManager::handleWindowEvent(SDL_WindowEvent window)
{
	switch (window.event) {
        case SDL_WINDOWEVENT_SHOWN:
            debug::log("Window %d shown", window.windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            debug::log("Window %d hidden", window.windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            debug::log("Window %d exposed", window.windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
            debug::log("Window %d moved to %d,%d",
                    window.windowID, window.data1,
                    window.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            debug::log("Window %d resized to %dx%d",
                    window.windowID, window.data1,
                    window.data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            debug::log("Window %d size changed to %dx%d",
                    window.windowID, window.data1,
                    window.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            debug::log("Window %d minimized", window.windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            debug::log("Window %d maximized", window.windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            debug::log("Window %d restored", window.windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
            debug::log("Mouse entered window %d",
                    window.windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            debug::log("Mouse left window %d", window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            debug::log("Window %d gained keyboard focus",
                    window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            debug::log("Window %d lost keyboard focus",
                    window.windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            debug::log("Window %d closed", window.windowID);
            break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
        case SDL_WINDOWEVENT_TAKE_FOCUS:
            debug::log("Window %d is offered a focus", window.windowID);
            break;
        case SDL_WINDOWEVENT_HIT_TEST:
            debug::log("Window %d has a special hit test", window.windowID);
            break;
#endif
        default:
            debug::log("Window %d got unknown event %d",
                    window.windowID, window.event);
            break;
        }
}

const InputState& InputManager::getState() const {
	return _state;
}