#pragma once
#include <memory>
#include "src/basic_types.h"
namespace engine::sdl 
{
	class Window : NotCopyOrMoveable {
	public:
		Window(i32 width, i32 height);
		~Window();
		void swapBuffer();
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl = {};
	};
}