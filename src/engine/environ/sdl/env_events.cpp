#include "env_events.h"
#include <SDL.h>
#include <vector>

namespace engine::sdl
{
	struct EnvEvents::Impl {
		std::vector<SDLEventCallback> mouseEventCallbacks;
		std::vector<SDLEventCallback> keyEventCallbacks;

		bool quitSignalReceived = false;
	};

	EnvEvents::EnvEvents() : _impl(std::make_unique<Impl>())
	{
	}
	EnvEvents::~EnvEvents() = default;
	void EnvEvents::attachMouseEventCallback(SDLEventCallback && c)
	{
		_impl->mouseEventCallbacks.emplace_back(std::move(c));
	}
	void EnvEvents::attachKeyEventCallback(SDLEventCallback && c)
	{
		_impl->keyEventCallbacks.emplace_back(std::move(c));
	}
	void EnvEvents::pumpEvents()
	{
		SDL_PumpEvents();
	}
	void EnvEvents::dispatchEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:// fallthrough
			case SDL_MOUSEBUTTONUP: // fallthrough
			case SDL_MOUSEMOTION: // fallthrough
			case SDL_MOUSEWHEEL: // fallthough
				for (auto& c : _impl->mouseEventCallbacks) {
					c(event);
				}
				break;
			case SDL_KEYDOWN: // fallthough
			case SDL_KEYUP: // fallthough
				for (auto& c : _impl->keyEventCallbacks) {
					c(event);
				}
				break;
			case SDL_QUIT:
				_impl->quitSignalReceived = true;
				break;
			}
			
		}
	}
	bool EnvEvents::quitSignalReceived() const
	{
		return _impl->quitSignalReceived;
	}
}