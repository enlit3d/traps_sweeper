#pragma once

#include "src/basic_types.h"
#include <memory>
#include <vector>

namespace engine::memory
{
	class MemAllocator {
		struct MemNode {
			MemNode(i32 size) :size(size) {}
			i32 size = 0;
			friend bool operator< (const MemNode& a, const MemNode& b) {
				return a.size < b.size;
			}
			std::vector<i32> _locs = {};
		};
	public:
		struct MemRange {
			i32 offset = 0;
			i32 size = 0;
		};
		MemAllocator(i32 maxCapacity=-1);
		bool hasRoom(i32 reqSize) const;
		MemRange allocRoom(i32 reqSize);
		void freeRoom(const MemRange& r);
		i32 used() const;
	private:
		i32 _used = 0;
		i32 _maxCapacity = -1;
		std::vector<MemNode> _nodes = {};
	};

}