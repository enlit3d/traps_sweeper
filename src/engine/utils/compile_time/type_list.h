#pragma once

#include "compile_time.h"
#include <type_traits>

namespace engine::compile_time
{
	template <typename... Ts>
	struct TypeList {
	private: // implementation details
		template <typename T, typename... Ts>
		struct HeadTail {
			static_assert(sizeof...(Ts) > 0, "Cannot get head or tail of empty TypeList");
			using Head = T;
			using Tail = TypeList<Ts...>;
		};

		template <i32 Index, typename T>
		struct TypeAt;

		template <i32 Index, typename T, typename... Ts>
		struct TypeAt<Index, TypeList<T, Ts...>>
			: TypeAt<Index - 1, TypeList<Ts...>> {};

		template <typename T, typename... Ts>
		struct TypeAt<0, TypeList<T, Ts...>> {
			using type = T;
		};

	public:
		using Head = typename HeadTail<Ts...>::Head;

		using Tail = TypeList<typename HeadTail<Ts...>::Tail>;

		template <typename... Types>
		using Push = TypeList<Types..., Ts...>;

		template <typename... Types>
		using PushBack = TypeList<Ts..., Types...>;

		template <i32 Index>
		using At = typename TypeAt<Index, TypeList<Ts...>>::type;

		template <typename T>
		static constexpr bool contains = compile_time::contains<T, Ts...>();

		template <typename T>
		static constexpr i32 indexOf = compile_time::indexOf<T, Ts...>();

		static constexpr auto size = sizeof...(Ts);

		// experimental stuff:

		template <typename Callable>
		static constexpr void callEach(Callable&& call) { // invoke a callable on each type, probably not very useful until c++20 for templated lambdas
			(call.template operator()<Ts> (), ...);
		};
	};
}