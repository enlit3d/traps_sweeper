#pragma once

#include <memory>
#include <functional>

#include "src/basic_types.h"

namespace engine::sdl
{
	class MainLoop;

	class Cursor {
		using Callback = std::function<void(const Cursor&)>;
	public:
		number x = 0.0;
		number y = 0.0;
		number dx = 0.0;
		number dy = 0.0;

		enum Button { NONE, LEFT, RIGHT, MIDDLE }; // enum rather than enum class on purpose so you can do cursor.RIGHT or cursor::RIGHT.
		Button button = Button::NONE;
	public:
		Cursor(MainLoop& loop);
		~Cursor();

		void onDown(Callback&& c);
		void onUp(Callback&& c);
		void onMove(Callback&& c);
		void onWheel(Callback&& c);

	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}