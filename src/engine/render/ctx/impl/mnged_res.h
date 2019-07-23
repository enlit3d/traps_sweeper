#pragma once
#include "src/engine/collections/handle_map_stable.h"
#include <type_traits>

namespace engine::render {
	template <typename T>
	class ResPool {
		static_assert(std::is_default_constructible_v<T>, "Type must be default constructible.");

		using Index = i32;
		struct Data {
			T elem = {};
			i32 refCount = 0;
			Index index = 0;
		};
		using Container = collections::Blocks<Data, 32>;

	public:
		class Ref {
		public:
			Ref(ResPool& pool, Data& elem) 
				: _pool(&pool), _data(&elem)
			{
				_data->refCount++;
			}
			~Ref() {
				if (_pool) {
					_data->refCount--;
					if (_data->refCount <= 0) {
						_pool->disposeData(*_data);
					}
				}
			
			}
			Ref(Ref&& other) noexcept
			{
				*this = std::move(other);
			}
			Ref& operator=(Ref&& other) noexcept {
				std::swap(_data, other._data);
				std::swap(_pool, other._pool);
				return *this;
			}
			Ref(Ref&) = delete;
			Ref& operator=(Ref&) = delete;
			Ref copy() { // explicit copy to avoid accidental incre/decrement of _refCount
				return Ref{ _pool, *_data };
			}
			T& operator* () {
				return _data->elem;
			}
			T* operator->() {
				return &(_data->elem);
			}
		private:
			Data* _data = nullptr;
			ResPool* _pool = nullptr;
			
		};

		Ref acquire() {
			auto& d = this->acquireData();
			return Ref{ *this, d };
		}

	private:
		friend class Ref;

		Data& acquireData() {
			if (_freeIndices.size() > 0) {
				auto index = _freeIndices.back();
				_freeIndices.pop_back();
				return _container[index];
			}
			else {
				auto index = static_cast<Index>(_container.count());
				auto& d = _container.push();
				d.index = index;
				return _container[index];
			}
		}

		void disposeData(Data& data) {
			_freeIndices.push_back(data.index);
		}

	private:
		Container _container = {};
		std::vector<Index> _freeIndices = {}; 
	};

	template <typename T>
	using Ref = typename ResPool<T>::Ref;
}