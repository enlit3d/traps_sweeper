#include "conversions.h"
#include <SDL_image.h>

#include <cassert>
#include <iostream>

namespace engine::env::conversions {
	void initSDLImage() {
		auto flags = IMG_INIT_JPG | IMG_INIT_PNG;
		auto result = IMG_Init(flags);
		assert((result & flags));
	}

	render::ImageData toImageDataAndDelete(SDL_Surface* surf)
	{
		initSDLImage();

		auto bytesPerPixel = surf->format->BytesPerPixel;
		auto targetFormat = SDL_PIXELFORMAT_ABGR8888; // always target 32 bit textures as 24 bit textures may have to be emulated in driver and is slower
		auto convertedSurf = SDL_ConvertSurfaceFormat(surf, targetFormat, 0);
		if (convertedSurf) {
			SDL_FreeSurface(surf);
			surf = convertedSurf;
		}
		else {
			std::cout << "WARNING: Failed to convert SDL_Surface to appropriate format. \n";
		}

		auto imageData = render::ImageData{ surf->w, surf->h, surf->format->BytesPerPixel, static_cast<byte*>(surf->pixels) };
		SDL_FreeSurface(surf);
		return imageData;
	}
}