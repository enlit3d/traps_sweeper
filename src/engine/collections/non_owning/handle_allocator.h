#pragma once

#include <vector>
#include "src/basic_types.h"

namespace engine::collections 
{
	class HandleAllocator {
	public:
		using Index = i32;

		struct SwapIndices {
			u32 first;
			u32 second;
		};

		struct Handle {
			u32 slotIndex;
			u32 id;
		};

		const Handle acquire();
		Index get(Handle h);
		Handle getHandle(Index index);
		SwapIndices dispose(Handle h);
		bool verify(Handle h) const;
		i32 count() const { return static_cast<i32>(this->handleCount); }
	private:
		struct Slot {
			u32 index;
			u32 id = 0;
		};

		std::vector<Slot> slots = {};
		std::vector<u32> indexToSlot = {}; // required to make swap-delete work
		u32 handleCount = 0;
		u32 ids = 1; // starts at 1 so id = 0 is a null value
		u32 nextFreeIndex = std::numeric_limits<u32>::max();
	};
}