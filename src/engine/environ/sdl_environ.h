#pragma once
#include "sdl/window.h"
#include "sdl/main_loop.h"
#include "sdl/cursor.h"
#include "sdl/keyboard.h"
namespace engine::env
{
	using namespace engine;
	using Window = typename sdl::Window;
	using MainLoop = typename sdl::MainLoop;
	using Cursor = typename sdl::Cursor;
	using Keyboard = typename sdl::Keyboard;
}