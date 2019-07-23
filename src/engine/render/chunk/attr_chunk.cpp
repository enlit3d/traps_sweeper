#include "attr_chunk.h"
#include "src/engine/collections/memory/mem_allocator.h"
#include "impl/chunk_mem.h"
#include "impl/gpu_buffers.h"

#include "../opengl/gl_shader.h"
#include <vector>


namespace engine::render
{
	static auto TEMP_MEMORY = ManagedMemory{};

	struct AttrChunk::Impl {
		Impl() {
		
		}
		void init(Layout layout) {
			_gpuBuffers.init(layout);

			_uploads._regions.clear();
			_uploads._usedBlocks.clear();
		};
		void upload() {
			if (_uploads._regions.size() == 0) { return; }

			// resize if needed
			if (_gVertCapacity < _vertSlots.used() || _gIndexCapacity < _indexSlots.used()) {
				_gVertCapacity = _vertSlots.used();
				_gIndexCapacity = _indexSlots.used();
				for (auto& buf : _gpuBuffers) {
					if (buf._settings.isIndices) {
						buf._buffer.resize(_gIndexCapacity*buf.bytesPerElem());
					}
					else {
						buf._buffer.resize(_gVertCapacity*buf.bytesPerElem());
					}
				}
			}

			// upload
			for (UploadRegion& r : _uploads._regions) {
				auto& buf = _gpuBuffers[r._key];
				auto bytePerVertex = buf.bytesPerElem();
				auto offset = r._target.offset*bytePerVertex;
				auto bytes = r._target.size*bytePerVertex;
				buf._buffer.write(r._data.data(), offset, bytes);
			}

			// free the temp resources
			_uploads._regions.clear();
			_uploads._usedBlocks.clear();
		}

		memory::RawView getWriteBuffer(memory::MemAllocator::MemRange r, i32 index) {
			auto& settings = _gpuBuffers[index]._settings;
			auto bytes = r.size * settings.bytesPerVertex();

			auto& uploads = _uploads;
			if (uploads._usedBlocks.size() == 0 || !uploads._usedBlocks.back()->hasRoom(bytes)) {
				uploads._usedBlocks.emplace_back(TEMP_MEMORY.reqBlock(bytes));
			}

			auto& block = uploads._usedBlocks.back();
			auto v = block->req(bytes);
			uploads._regions.push_back(Impl::UploadRegion{ index, v, r });

			return v;
		}

		struct UploadRegion {
			Layout::Index _key;
			memory::RawView _data;
			memory::MemAllocator::MemRange _target;
		};

		struct UploadingData {
			std::vector<MemBlock::Handle> _usedBlocks = {};
			std::vector<UploadRegion> _regions = {};
		};

		DrawRanges _drawRanges = {};
		memory::MemAllocator _vertSlots = {65535};
		memory::MemAllocator _indexSlots = {};
		UploadingData _uploads = {};
		GPUBuffers _gpuBuffers = {};
		i32 _gVertCapacity = 0;
		i32 _gIndexCapacity = 0;
	};

	AttrChunk::AttrChunk()
		: _impl(std::make_unique<Impl>())
	{
	}

	AttrChunk::~AttrChunk() = default;

	bool AttrChunk::canFit(i32 vCount, i32 iCount) const
	{
		return _impl->_vertSlots.hasRoom(vCount) && _impl->_indexSlots.hasRoom(iCount);
	}

	void AttrChunk::init(Layout layout) {
		_impl->init(layout);
	}

	ChunkPointer AttrChunk::request(i32 vCount, i32 iCount)
	{
		if (vCount == 0 || iCount == 0) {
			return ChunkPointer{ *this, {0,0}, {0,0} };
		}
		auto vSeg = _impl->_vertSlots.allocRoom(vCount);
		if (vSeg.size == 0) { throw; }
		auto iSeg = _impl->_indexSlots.allocRoom(iCount);
		if (iSeg.size == 0) { throw; }
		_impl->_drawRanges.insert(iSeg.offset, iSeg.offset + iSeg.size);
		return ChunkPointer{ *this, vSeg, iSeg };
	}

	void AttrChunk::dispose(const ChunkPointer& ptr)
	{
		if (!ptr) { return; }
		_impl->_vertSlots.freeRoom(ptr._vSeg);
		_impl->_indexSlots.freeRoom(ptr._iSeg);
		_impl->_drawRanges.remove(ptr._iSeg.offset, ptr._iSeg.offset + ptr._iSeg.size); 
		//ptr._chunk = nullptr;
	}

	void AttrChunk::upload()
	{
		_impl->upload();
	}

	bool AttrChunk::hasBuffer(str_ref attrib)
	{
		return _impl->_gpuBuffers.indexOf(attrib) >= 0;
	}

	memory::RawView AttrChunk::writeBuffer(const ChunkPointer & pt, str_ref attrib)
	{
		auto k = _impl->_gpuBuffers.indexOf(attrib);
		if (k < 0) { throw; }
		return _impl->getWriteBuffer(pt._vSeg, k);
	}

	memory::RawView AttrChunk::writeIndexBuffer(const ChunkPointer & pt)
	{
		auto k = _impl->_gpuBuffers.indexOf(""); // implementation detail: index buffer is unnamed ("")
		if (k < 0) { throw; }
		return _impl->getWriteBuffer(pt._iSeg, k);
	}

	void AttrChunk::toggleDrawRange(i32 start, i32 end, bool toDraw)
	{
		if (toDraw) {
			_impl->_drawRanges.insert(start, end);
		}
		else {
			_impl->_drawRanges.remove(start, end);
		}
	}

	const GPUBuffers& AttrChunk::getGPUBuffers() const
	{
		return _impl->_gpuBuffers;
	}

	const DrawRanges& AttrChunk::drawRanges() const
	{
		return _impl->_drawRanges;
	}

	ChunkPointer::ChunkPointer()
	{
	}

	ChunkPointer::ChunkPointer(AttrChunk & chunk, MemRange v, MemRange i)
		: _chunk(&chunk), _vSeg(v), _iSeg(i)
	{
	}

	ChunkPointer::ChunkPointer(ChunkPointer&& other) noexcept 
	{
		std::swap(_chunk, other._chunk);
		std::swap(_vSeg, other._vSeg);
		std::swap(_iSeg, other._iSeg);
	}

	ChunkPointer::~ChunkPointer()
	{
		if (_chunk) {
			_chunk->dispose(*this);
		}
	}

	bool ChunkPointer::hasBuffer(str_ref attrib)
	{
		return _chunk->hasBuffer(attrib);
	}

	void ChunkPointer::toggleDrawRange(bool draw)
	{
		_chunk->toggleDrawRange(_iSeg.offset, _iSeg.offset + _iSeg.size, draw);
	}

	i32 ChunkPointer::vCount() const
	{
		return _vSeg.size;
	}

	i32 ChunkPointer::iCount() const
	{
		return _iSeg.size;
	}

	i32 ChunkPointer::vOffset() const
	{
		return _vSeg.offset;
	}

	i32 ChunkPointer::iOffset() const
	{
		return _iSeg.offset;
	}

	ChunkPointer::operator bool() const
	{
		return _chunk != nullptr && _vSeg.size > 0;
	}

	template<typename T>
	memory::ArrayView<T> ChunkPointer::getTempWriteBuffer(str_ref attrib)
	{
		return _chunk->writeBuffer(*this, attrib).view<T>();
	}

	template<typename T>
	memory::ArrayView<T> ChunkPointer::getTempWriteIndexBuffer()
	{
		return _chunk->writeIndexBuffer(*this).view<T>();
	}

	template <typename T>
	void ChunkPointer::writeData(str_ref attrib, const memory::ArrayView<T>& src) {
		this->getTempWriteBuffer<T>(attrib) = src;
	}

	template<typename T>
	void ChunkPointer::writeIndices(const memory::ArrayView<T>& src)
	{
		auto dest = this->getTempWriteIndexBuffer<T>();
		auto offset = this->_vSeg.offset;
		for (auto i = 0; i < src.count(); ++i) {
			dest[i] = src[i] + offset;
		}
	}

	template memory::ArrayView<f32> ChunkPointer::getTempWriteBuffer(str_ref attrib);
	template void ChunkPointer::writeData(str_ref attrib, const memory::ArrayView<f32>& src);

	template memory::ArrayView<u16> ChunkPointer::getTempWriteIndexBuffer();
	template void ChunkPointer::writeIndices(const memory::ArrayView<u16>& data);
}