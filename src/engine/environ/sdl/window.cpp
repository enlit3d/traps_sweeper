#include "window.h"
#include <SDL.h>
#define GLEW_STATIC 
#include <GL/glew.h>
#include <iostream>


namespace engine::sdl
{
	struct Window::Impl
	{
		struct WinDeleter {
			void operator()(SDL_Window* ptr) {
				SDL_DestroyWindow(ptr);
			}
		};
		std::unique_ptr<SDL_Window, WinDeleter> window;
	};

	Window::Window(i32 width, i32 height) : _impl(std::make_unique<Impl>())
	{
		SDL_Init(SDL_INIT_VIDEO);
		_impl->window.reset(SDL_CreateWindow(
			"SDL2Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
		));

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		auto gl = SDL_GL_CreateContext(_impl->window.get());
		if (!gl) {
			std::cout << "OpenGL context creation failed. " << SDL_GetError();
			throw;
		}

		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			std::cout << "Initializing GLEW failed. " << glewGetErrorString(glewError);
			throw;
		}
	}
	Window::~Window() = default;
	void Window::swapBuffer()
	{
		SDL_GL_SwapWindow(_impl->window.get());
	}
}