#pragma once

#include "src/basic_types.h"

namespace engine::gui {
	struct Vec2 {
		i32 x = 0;
		i32 y = 0;
	};

	struct Rect {
		i32 x = 0;
		i32 y = 0;
		i32 w = 0;
		i32 h = 0;

		bool contains(Vec2 v) const {
			return this->x <= v.x && this->x + this->w >= v.x && this->y <= v.y && this->y + this->h >= v.y;
		};
	};

	class Renderable : NotCopyOrMoveable { 	// interface
	public:
		Renderable() = default;
		virtual ~Renderable() = default;
		virtual void setBounds(Rect rect) = 0;
		virtual void setVis(bool) = 0;
	};
}