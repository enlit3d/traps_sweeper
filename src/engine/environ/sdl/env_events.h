#pragma once

#include "src/basic_types.h"
#include <memory>
#include <functional>

union SDL_Event;

namespace engine::sdl
{
	class EnvEvents {
	public:
		using SDLEventCallback = std::function<void(SDL_Event&)>;
		EnvEvents();
		~EnvEvents();
		
		void attachMouseEventCallback(SDLEventCallback&& c);
		void attachKeyEventCallback(SDLEventCallback&& c);
		void pumpEvents();
		void dispatchEvents();
		bool quitSignalReceived() const;
	
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}