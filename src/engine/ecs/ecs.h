#pragma once

#include "src/basic_types.h"
#include "src/engine/collections/non_owning/handle_allocator.h"
#include "src/engine/collections/flat_map.h"
#include "src/engine/collections/bit_set.h"
#include "src/engine/utils/compile_time/compile_time.h"
#include <vector>
#include <memory>

namespace engine::ecs
{
	using namespace engine;

	using HandleAllocator = collections::HandleAllocator;
	template <typename K, typename V>
	using FlatMap = collections::FlatMap<K, V>;


	template <typename... Components>
	class TypeRegistry {
	public: 
		using TypeId = i32;
		
		template <typename T>
		constexpr static TypeId getId() {
			static_assert(compile_time::contains<T, Components...>(), "Unknown component type");
			return static_cast<TypeId>(compile_time::indexOf<T, Components...>());
		}

		constexpr static i32 typeCount() {
			return static_cast<i32>(compile_time::count<Components...>());
		}
	public:
		using BitMask = collections::BitSet<TypeRegistry::typeCount()>;
	private:
		template <typename T, typename... Comps>
		constexpr static BitMask makeBitMaskImpl(BitMask mask) {
			mask.set(TypeRegistry::getId<T>(), true);
			if constexpr (sizeof...(Comps) > 0) {
				return TypeRegistry::makeBitMaskImpl<Comps...>(mask);
			}
			else {
				return mask;
			}
		}
	public:
		template <typename... Components>
		constexpr static BitMask makeBitMask() {
			if constexpr (sizeof...(Components) > 0) {
				return makeBitMaskImpl<Components...>(BitMask{});
			}
			else {
				return BitMask{};
			}
		}
	};

	template <typename... Components>
	class EntityGroup : NotCopyOrMoveable {
	public:
		template <typename T>
		using Container = std::vector<T>;
		using TypeReg = typename TypeRegistry<Components...>;
		using TypeMask = typename TypeReg::BitMask;

	private:
		class ContainerWrapperBase {
		public:
			virtual ~ContainerWrapperBase() {};
		};

		template <typename T>
		class ContainerWrapper : public ContainerWrapperBase {
		public:
			Container<T>& get() { return data; }
		private:
			Container<T> data = {};
		};

	public:
		EntityGroup() {};

		template <typename... Ts>
		void allocAs() {
			if (this->containers.size() != 0) { return; }
			this->allocStorageImpl<Ts...>();
			this->typeMask = TypeReg::makeBitMask<Ts...>();
		}

		template <typename T>
		Container<T>& getStorage() {
			constexpr auto id = TypeReg::getId<T>();
			return static_cast<ContainerWrapper<T>&>(*this->containers.get(id)).get();
		}

		using Handle = typename HandleAllocator::Handle;

		template <typename... Ts>
		Handle acquire() {
			auto h = this->indirection.acquire();
			auto index = this->indirection.get(h);

			if (index >= this->usedCapacity) {
				this->allocSpace<Ts...>();
				++this->usedCapacity;
			}

			return h;
		}

		Handle getHandle(HandleAllocator::Index index) {
			return this->indirection.getHandle(index);
		}

		template <typename T>
		T& get(Handle h) {
			// TODO: dealing with invalid handles?
			auto index = this->indirection.get(h);
			return this->getStorage<T>()[index];
		}

		void disposeNow(Handle h) {
			auto swapIndices = this->indirection.dispose(h);
			if (swapIndices.first != swapIndices.second) {
				this->disposeImpl<Components...>(swapIndices.first, swapIndices.second);
			}
		}

		TypeMask getMask() const {
			return this->typeMask;
		}

		auto count() const {
			return this->indirection.count();
		}

	private:
		HandleAllocator indirection = {};
		FlatMap<typename TypeReg::TypeId, std::unique_ptr<ContainerWrapperBase>> containers = {};
		TypeMask typeMask = {};
		i32 usedCapacity = 0;
	private:
		
		template <typename T, typename... Ts>
		void allocStorageImpl() {
			constexpr auto id = TypeReg::getId<T>();
			this->containers.emplace(id, new ContainerWrapper<T>{});
			if constexpr (sizeof...(Ts) > 0) {
				allocStorageImpl<Ts...>();
			}
		}

		template <typename T, typename... Ts>
		void allocSpace() {
			this->getStorage<T>().emplace_back();
			if constexpr (sizeof...(Ts) > 0) {
				allocSpace<Ts...>();
			}
		}

		template <typename T, typename... Ts>
		void disposeImpl(u32 a, u32 b) {
			auto& c = this->getStorage<T>();
			std::swap(c[a], c[b]);
			if constexpr (sizeof...(Ts) > 0) {
				disposeImpl<Ts...>(a, b);
			}
		}
	};

	template <typename... Components>
	class EntityHandle {
		using EntityGroup = typename EntityGroup<Components...>;
		using Handle = typename EntityGroup::Handle;
	public:
		EntityHandle(Handle handle, EntityGroup& group) : handle(handle), group(group) {};
		auto getMask() {
			return this->group.getMask();
		}
		template <typename T>
		T& get() {
			return this->group.get<T>(this->handle);
		}
		template <typename... Ts>
		bool has() const {
			constexpr auto requested = EntityGroup::TypeReg::makeBitMask<Ts...>();
			auto valid = this->getMask();
			return (valid & requested) == requested;
		}
	private:
		EntityGroup& group;
		Handle handle;
	};

	template <typename... Components>
	class System {
		using EntityGroup = typename EntityGroup<Components...>;
		using EntityHandle = typename EntityHandle<Components...>;
	public:
		System() {}
		
		template <typename... Ts>
		EntityGroup& getGroup() {
			constexpr auto mask = EntityGroup::TypeReg::makeBitMask<Ts...>();
			if (!this->_entityGroups.has(mask)) {
				auto& g = this->_entityGroups.emplace(mask, new EntityGroup{});
				g->allocAs<Ts...>();
			}
			return *(this->_entityGroups.get(mask));
		}

		template <typename... Ts>
		EntityHandle makeEntity() {
			auto& g = this->getGroup<Ts...>();
			auto h = g.acquire<Ts...>();
			return EntityHandle{ h, g };
		}

		// Iteration

	private:
		template <typename... Ts>
		class Entity : NotCopyOrMoveable {
		public:
			Entity(EntityHandle handle, std::tuple<Ts&...> components)
				: handle(handle), components(components)
			{}

			EntityHandle handle;
			std::tuple<Ts&...> components;

			template <typename T>
			T& get() {
				static_assert(compile_time::contains<T, Ts...>(), "Entity does not contain component of requested type.");
				return std::get<T>(components);
			}
		};
		
		template <typename... Ts>
		class Iter {
		public:
			Iter(System& sys, i32 groupIndex, i32 elemIndex)
				: _sys(sys), _groupIndex(groupIndex), _elemIndex(elemIndex)
			{
				if (_groupIndex < groupCount()) {
					toNextValidGroup();
				}
			}

			bool operator !=(const Iter& other) const {
				return _groupIndex != other._groupIndex || _elemIndex != other._elemIndex;
			}

			Iter& operator++() {
				++_elemIndex;
				if (_elemIndex == _curGroup->count()) {
					_elemIndex = 0;
					++_groupIndex;
					toNextValidGroup();
				}
				return *this;
			}

			Entity<Ts...> operator*() {
				auto index = _elemIndex;
				auto handle = EntityHandle{_curGroup->getHandle(index), *_curGroup};
				auto containers = std::forward_as_tuple(_curGroup->getStorage<Ts>()...);
				auto t2 = compile_time::tupleTransform(
					containers,
					[index](auto& vec) {
						return std::ref(vec[index]);
					}
				);
				return Entity(handle, t2);
			}
			
		private:
			System& _sys;
			i32 _groupIndex;
			i32 _elemIndex;
			EntityGroup* _curGroup;

			i32 groupCount() {
				return static_cast<i32>(_sys._entityGroups.values().size());
			}

			void toNextValidGroup() {
				constexpr auto mask = EntityGroup::TypeReg::makeBitMask<Ts...>();
				while (_groupIndex < groupCount()) {
					if (_groupIndex == groupCount()) { break; }
					auto& ptr = _sys._entityGroups.values()[_groupIndex];
					EntityGroup& group = *ptr;
					if (group.count() == 0) { ++_groupIndex; continue; }
					if ((group.getMask() & mask) != mask) { ++_groupIndex; continue; }
					_curGroup = ptr.get();
					break;
				}
			}
		};
		
		template <typename... Ts>
		class IterPair {
		public:
			IterPair(System& sys) : _sys(sys) {};
			Iter<Ts...> begin() const {
				return Iter<Ts...>{ _sys, 0, 0 };
			}
			Iter<Ts...> end() const {
				return Iter<Ts...>{ _sys, static_cast<i32>(_sys._entityGroups.values().size()), 0 };
			}
		private:
			System& _sys;
		};


	public:
		template <typename... Ts>
		auto entities() {
			return IterPair<Ts...>{ *this };
		}


	public:

	private:
		FlatMap<typename EntityGroup::TypeMask, std::unique_ptr<EntityGroup>> _entityGroups = {}; // pointer to enforce EntityGroup address stability
	};
}