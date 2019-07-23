#pragma once

#include <memory>
#include <functional>

#include "src/basic_types.h"

namespace engine::sdl
{
	class MainLoop;

	class Keyboard {
		using Callback = std::function<void(const Keyboard&)>;
	public:
		Keyboard(MainLoop& loop);
		~Keyboard();

		void onDown(Callback&& c);
		void onUp(Callback&& c);

	public:
		enum Key : u8 {  // enum rather than enum class on purpose so you can do kb.A or kb::A.
			DONT_KNOW, // for not implemented keys...
			_0,
			_1,
			_2,
			_3,
			_4,
			_5,
			_6,
			_7,
			_8,
			_9,
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			Q,
			R,
			S,
			T,
			U,
			V,
			W,
			X,
			Y,
			Z,
			SPACE,
			UP,
			DOWN,
			LEFT,
			RIGHT
			//ENTER,
			//CTRL,
			//ALT,
			//ESC,
		};
		Key key;
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}