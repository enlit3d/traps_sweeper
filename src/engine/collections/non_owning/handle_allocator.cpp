#include "handle_allocator.h"
#include <cassert>

namespace engine::collections
{
	const HandleAllocator::Handle HandleAllocator::acquire()
	{
		auto id = this->ids++;
		u32 index = this->handleCount++;
		if (this->nextFreeIndex == std::numeric_limits<u32>::max()) { // no more free space
			auto slotIndex = static_cast<u32>(this->slots.size());
			slots.emplace_back(Slot{ index, id });
			this->indexToSlot.emplace_back(slotIndex);
			return Handle{ slotIndex, id };
		}
		else {
			u32 slotIndex = this->nextFreeIndex;
			auto& slot = this->slots[slotIndex];
			this->nextFreeIndex = slot.index;
			slot.index = index;
			slot.id = id;
			this->indexToSlot[slot.index] = slotIndex;
			return Handle{ slotIndex, id };
		}
		return Handle();
	}
	HandleAllocator::Index HandleAllocator::get(Handle h)
	{
		auto& slot = this->slots[h.slotIndex];
		assert(slot.id == h.id);
		return slot.index;
	}
	HandleAllocator::Handle HandleAllocator::getHandle(Index index)
	{
		auto slotIndex = this->indexToSlot[index];
		return Handle {
			slotIndex,
			this->slots[slotIndex].id
		};
	}
	HandleAllocator::SwapIndices HandleAllocator::dispose(Handle h)
	{
		if (!this->verify(h)) { return  HandleAllocator::SwapIndices{ 0,0 }; }
		auto& slot = slots[h.slotIndex];
		slot.id = 0; // fails the validate test for other handles that point to the slot
		auto index = slot.index;
		this->handleCount--;
		auto swappedIndex = this->handleCount;
		// swap the last elem with the disposed elem
		this->slots[this->indexToSlot[swappedIndex]].index = index;
		std::swap(this->indexToSlot[index], this->indexToSlot[swappedIndex]);
		// update the freelist
		slot.index = this->nextFreeIndex;
		this->nextFreeIndex = h.slotIndex;
		// return
		return HandleAllocator::SwapIndices{ index, swappedIndex };
	}
	bool HandleAllocator::verify(Handle h) const
	{
		return this->slots[h.slotIndex].id == h.id;
	}
}