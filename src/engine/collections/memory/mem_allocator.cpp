#include "mem_allocator.h"
#include <algorithm>

namespace engine::memory
{
	MemAllocator::MemAllocator(i32 maxCapacity) : _maxCapacity(maxCapacity)
	{
	}
	bool MemAllocator::hasRoom(i32 reqSize) const
	{
		auto testNode = MemNode{ reqSize };
		auto it = std::lower_bound(_nodes.begin(), _nodes.end(), testNode);
		if (it == _nodes.end() || it->_locs.size() == 0 || it->size * 0.8 >= reqSize) {
			if (_maxCapacity > 0 && _used + reqSize > _maxCapacity) { // no more room
				return false;
			}
		}
		return true;
	}
	MemAllocator::MemRange MemAllocator::allocRoom(i32 reqSize)
	{
		auto testNode = MemNode{reqSize };
		auto it = std::lower_bound(_nodes.begin(), _nodes.end(), testNode);
		if (it == _nodes.end() || it->_locs.size() == 0 || it->size * 0.8 >= reqSize) {
			if (_maxCapacity > 0 && _used + reqSize > _maxCapacity) { // no more room
				return { 0,0 };
			}
			else {
				// return new region
				auto node = MemRange{ _used, reqSize };
				_used += reqSize;
				return node;
			}
		}
		else {
			auto& node = *it;
			auto offset = node._locs.back();
			node._locs.pop_back();

			//TODO: maybe delete nodes with empty ._locs might be a good idea. Need to test.

			return MemRange{ offset, reqSize };
		}
	}
	void MemAllocator::freeRoom(const MemRange & r)
	{
		auto testNode = MemNode{ r.size };
		auto it = std::lower_bound(_nodes.begin(), _nodes.end(), testNode);
		if (it == _nodes.end() || it->size != r.size) {
			auto inserted = _nodes.emplace(it, r.size);
			inserted->_locs.emplace_back(r.offset);
		}
		else {
			it->_locs.emplace_back(r.offset);
		}
	}
	i32 MemAllocator::used() const
	{
		return _used;
	}
}