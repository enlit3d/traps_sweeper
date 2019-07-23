#include "main_loop.h"
#include "env_events.h"

#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>

namespace engine::sdl
{
	struct MainLoop::Impl {
		Impl() 
			: envEvents(std::make_unique<EnvEvents>())
		{
		}
		std::vector<UpdateCallback> fixedStepCallbacks = {};
		std::vector<UpdateCallback> flexStepCallbacks = {};

		number targetFPS = 60.0;
		number deltaT = 0.0;
		number accumT = 0.0;
		number timeStamp = 0.0;
		bool running = true;

		std::unique_ptr<EnvEvents> envEvents;
	};

	MainLoop::MainLoop() : _impl(std::make_unique<Impl>())
	{
	}

	MainLoop::~MainLoop() = default;

	number MainLoop::timeStamp() const
	{
		return _impl->timeStamp;
	}

	void MainLoop::start()
	{
		_impl->running = true;
		auto prevTick = std::chrono::steady_clock::now();
		while (_impl->running) {
			if (_impl->envEvents) {
				auto& evts = *(_impl->envEvents);
				evts.pumpEvents();
				evts.dispatchEvents();
				if (evts.quitSignalReceived()) {
					_impl->running = false;
				}
			}

			auto timeStep = 1.0 / _impl->targetFPS;
			auto curTick = std::chrono::steady_clock::now();
			auto ticks = curTick - prevTick;
			prevTick = curTick;
			auto elapsedTime = std::chrono::duration<double>(ticks).count();
			elapsedTime = std::min(0.1, elapsedTime);
			_impl->deltaT += elapsedTime;

			if (_impl->deltaT < timeStep * 0.9) {
				std::this_thread::sleep_for(std::chrono::duration<double>(timeStep*0.9 - _impl->deltaT));
				continue;
			}

			_impl->accumT += _impl->deltaT;

			while (_impl->accumT >= timeStep) {
				_impl->timeStamp += timeStep;
				
				for (const auto& c : _impl->fixedStepCallbacks) {
					c(timeStep, _impl->timeStamp);
				}

				_impl->accumT -= timeStep;
			}

			for (const auto& c : _impl->flexStepCallbacks) {
				c(_impl->deltaT, _impl->timeStamp + _impl->accumT);
			}

			_impl->deltaT = 0.0;
		}
	}

	void MainLoop::terminate()
	{
		_impl->running = false;
	}

	void MainLoop::flexStep(UpdateCallback&& fn)
	{
		_impl->flexStepCallbacks.emplace_back(std::move(fn));
	}

	void MainLoop::fixedStep(UpdateCallback&& fn)
	{
		_impl->fixedStepCallbacks.emplace_back(std::move(fn));
	}

	EnvEvents & MainLoop::manageEvents()
	{
		if (!_impl->envEvents) {
			_impl->envEvents = std::make_unique<EnvEvents>();
		}
		return *(_impl->envEvents);
	}

}