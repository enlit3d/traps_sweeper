#pragma once

#include <vector>
#include "src/basic_types.h"

namespace engine::render {
	class Packer {
	public:
		Packer() 
			: _used(), _maxSize() 
		{}

		Packer(i32 maxW, i32 maxH)
			: _used(), _maxSize(0,0, maxW, maxH)
		{}

		struct Region {
			i32 x = 0;
			i32 y = 0;
			i32 w = 0;
			i32 h = 0;

			Region() {};
			Region(i32 x, i32 y, i32 w, i32 h) :x{ x }, y{ y }, w{ w }, h{ h } {};

			explicit operator bool() {
				return w > 0 && h > 0;
			}
		};

		Region packRegion(i32 w, i32 h) { // can return a null region if out of space!
			auto* r = findRegion(w, h);
			if (!r) {
				if (_used.w + w < _used.h + h) { // try to keep a square...
					r = tryGrowRight(w, h);
					if (!r) { r = tryGrowDown(w, h); }
				}
				else {
					r = tryGrowDown(w, h);
					if (!r) { r = tryGrowRight(w, h); }
				}
				if (!r) { return {}; }
			}

			return (r->h - h > r->w - w) ? cutHorizontal(*r, w, h) : cutVertical(*r, w, h);
		}

		i32 w() const {
			return _used.w;
		}

		i32 h() const {
			return _used.h;
		}

	private:
		Region* findRegion(i32 w, i32 h) {
			// can return nullptr;
			auto canFit = [](Region& r, i32 w, i32 h) {
				return w <= r.w && h <= r.h;
			};
			auto smallestSize = std::numeric_limits<i32>::max();
			Region* selectedRegion = nullptr;
			for (auto& r : _freeRegions) {
				if (canFit(r, w, h)) {
					auto size = r.w * r.h;
					if (size < smallestSize) {
						smallestSize = size;
						selectedRegion = &r;
					}
				}
			}
			return selectedRegion;
		}

		Region* tryGrowRight(i32 w, i32 h) {
			// can return nullptr;
			auto newWidth = _used.w + w;
			//newWidth = i3232RoundToNextPowerOf2(newWidth);
			if (_maxSize && newWidth > _maxSize.w) { return nullptr; }

			if (h > _used.h) {
				if (_maxSize && h > _maxSize.h) {
					return nullptr;
				}
				if (_used.w != 0) {
					_freeRegions.emplace_back(0, _used.h, _used.w, h - _used.h);
				}
				_used.h = h;
			}

			_freeRegions.emplace_back(_used.w, 0, newWidth - _used.w, _used.h);
			_used.w = newWidth;
			return &_freeRegions.back();
		}

		Region* tryGrowDown(i32 w, i32 h) {
			// can return nullptr;
			auto newHeight = _used.h + h;
			//newHeight = i3232RoundToNextPowerOf2(newHeight);
			if (_maxSize && newHeight > _maxSize.h) { return nullptr; }

			if (w > _used.w) {
				if (_maxSize && w > _maxSize.w) { return nullptr; }
				if (_used.h != 0) {
					_freeRegions.emplace_back(_used.w, 0, w - _used.w, _used.h);
				}
				_used.w = w;
			}

			_freeRegions.emplace_back(0, _used.h, _used.w, newHeight - _used.h);
			_used.h = newHeight;
			return &_freeRegions.back();
		}

		Region cutVertical(Region& r, i32 w, i32 h) {
			/*
				-----------------
				|		|		|
				|RETURN	|		|
				|REGION	| RIGHT	|
				|-------| REGION|
				|LEFT	|		|
				|REGION |		|
				-----------------
			*/

			Region result{ r.x, r.y, w, h };
			Region rightRegion{ r.x + w, r.y, r.w - w, r.h };
			auto& leftRegion = r;
			leftRegion = { r.x, r.y + h, w, r.h - h };

			if (leftRegion) {
				if (rightRegion) { // keep both left and right
					_freeRegions.push_back(rightRegion);
				}
				else {  // keep only left - do nothing

				}
			}
			else {
				if (rightRegion) { // only keep right - need to copy data from rightRegion to left
					leftRegion = rightRegion;
				}
				else { // discard both, need to remove leftRegion
					_freeRegions.erase(_freeRegions.begin() + (&leftRegion - _freeRegions.data()));
				}
			}
			return result;
		}

		Region cutHorizontal(Region& r, i32 w, i32 h) {
			/*
				-----------------
				|		|		|
				|RETURN	| TOP	|
				|REGION	|REGION	|
				|---------------|
				|	BOTTOM		|
				|	REGION 		|
				-----------------
			*/

			Region result{ r.x, r.y, w, h };
			Region botRegion{ r.x, r.y + h, r.w, r.h - h };
			auto& topRegion = r;
			topRegion = { r.x + w, r.y, r.w - w, h };

			if (topRegion) {
				if (botRegion) { // need to add botRegion to list
					_freeRegions.push_back(botRegion);
				}
				else { // do nothing

				}
			}
			else {
				if (botRegion) { // only keep botRegion, need to copy data over
					topRegion = botRegion;
				}
				else { // discard both, need to remove r from list
					_freeRegions.erase(_freeRegions.begin() + (&topRegion - _freeRegions.data()));
				}
			}

			return result;
		}
	private:
		std::vector<Region> _freeRegions;
		Region _used;
		Region _maxSize;
	};
}