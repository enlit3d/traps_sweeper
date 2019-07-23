#pragma once
#include <memory>
#include <functional>
#include "src/basic_types.h"

namespace engine::sdl
{
	class EnvEvents;

	class MainLoop {
	public:
		using UpdateCallback = std::function<void(number dt, number ts)>;

		MainLoop();
		~MainLoop();

		number timeStamp() const;
		void start();
		void terminate();

		void flexStep(UpdateCallback&& fn);
		void fixedStep(UpdateCallback&& fn);

		EnvEvents& manageEvents();
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl = {};
	};
}