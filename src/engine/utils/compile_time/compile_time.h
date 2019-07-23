#pragma once

#include <type_traits>
#include "src/basic_types.h"

namespace engine::compile_time
{

	template <typename... Ts>
	constexpr i32 count() {
		return static_cast<i32>(sizeof...(Ts));
	}
	
	template <typename T, typename... Ts>
	constexpr bool contains() {
		return std::disjunction_v<std::is_same<T, Ts>...>;
	}

	namespace {
		template <i32 Index, typename T, typename Head, typename... Tail>
		constexpr i32 indexOfImpl() {
			if constexpr (std::is_same_v<T, Head>) { return Index; }
			//else if constexpr (count<Tail...>() == 0) { static_assert(false, "Type is not in list"); }
			else { return indexOfImpl<Index + 1, T, Tail...>(); }
		}

		template <class Fn, typename Tuple, size_t... Ts>
		auto tupleTransformImpl(Tuple tuple, Fn fn, std::index_sequence<Ts...>) {
			return std::make_tuple(
				fn(std::get<Ts>(tuple))...
			);
		}
	}


	template <typename T, typename... Ts>
	constexpr i32 indexOf() {
		return indexOfImpl<0, T, Ts...>();
	}

	template <class Fn, typename...Types>
	auto tupleTransform(const std::tuple<Types...>& tuple, Fn fn) {
		return tupleTransformImpl(
			tuple, fn, std::make_index_sequence<sizeof...(Types)>{}
		);
	}

	template <class Fn, typename...Types>
	auto callEach(const std::tuple<Types...>& tuple, Fn fn) {
		std::apply(fn, tuple);
	}


}