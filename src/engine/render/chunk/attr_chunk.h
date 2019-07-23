#pragma once

#include "src/engine/collections/memory/raw_buffer.h"
#include "src/engine/collections/memory/mem_allocator.h"
#include "src/engine/collections/segments_list.h"
#include "src/string_type.h"
#include "src/basic_types.h"
#include "layout.h"
#include <memory>

namespace engine::render
{
	class AttrChunk;
	class GPUBuffers;

	using DrawRanges = collections::SegmentsList<i32>;

	class ChunkPointer : NotCopyable {
		using MemRange = memory::MemAllocator::MemRange;
		friend class AttrChunk;
	public:
		ChunkPointer();
		ChunkPointer(AttrChunk& chunk, MemRange v, MemRange i);
		ChunkPointer(ChunkPointer&&) noexcept;
		~ChunkPointer();

		bool hasBuffer(str_ref attrib);

		template <typename T>
		void writeData(str_ref attrib, const memory::ArrayView<T>& data);

		template <typename T>
		void writeIndices(const memory::ArrayView<T>& data);
		
		template <typename T>
		memory::ArrayView<T> getTempWriteBuffer(str_ref attrib);

		template <typename T>
		memory::ArrayView<T> getTempWriteIndexBuffer();

		void toggleDrawRange(bool draw);

		i32 vCount() const;
		i32 iCount() const;
		i32 vOffset() const;
		i32 iOffset() const;

		AttrChunk* operator->() {
			return _chunk;
		}
		AttrChunk& operator*() {
			return *_chunk;
		}

		explicit operator bool() const;
	private:
		AttrChunk* _chunk = nullptr;
		MemRange _vSeg = {};
		MemRange _iSeg = {};
	};

	class AttrChunk : NotCopyOrMoveable {
	public:
		AttrChunk();
		void init(Layout layout);
		~AttrChunk();
		bool canFit(i32 vCount, i32 iCount) const;
		ChunkPointer request(i32 vCount, i32 iCount);
		void upload();

		bool hasBuffer(str_ref attrib);
		memory::RawView writeBuffer(const ChunkPointer& seg, str_ref attrib); 
		memory::RawView writeIndexBuffer(const ChunkPointer& seg);

		const GPUBuffers& getGPUBuffers() const;
		const DrawRanges& drawRanges() const;
	private:
		friend class ChunkPointer;
		void dispose(const ChunkPointer& ptr);
		void toggleDrawRange(i32 start, i32 end, bool toDraw);

	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}