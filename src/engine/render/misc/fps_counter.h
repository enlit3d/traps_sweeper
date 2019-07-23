#pragma once
#include "src/ext/math.h"

namespace engine::render
{
	class FPSCounter {
	public:
		void inline tick(f32 dt) {
			_prevAvg = dt * 0.1f + _prevAvg * 0.9f;
		}

		f32 inline getFPS() {
			return 1.0f / _prevAvg;
		}

	private:
		f32 _prevAvg = 1.0f / 30.0f;
	};
}