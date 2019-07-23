#pragma once

#include <variant>

namespace engine {

	template<typename... Ts>
	using Variant = std::variant<Ts...>;

	template <class F, class Variant, size_t... N>
	decltype(auto) invoke_active(F&& f, Variant&& v, std::index_sequence<N...>)
	{
		using Ret = decltype(std::invoke(std::forward<F>(f), std::get<0>(std::forward<Variant>(v))));

		if constexpr (!std::is_same_v<void, Ret>)
		{
			Ret ret;
			if (!((v.index() == N ? (ret = std::invoke(std::forward<F>(f), std::get<N>(std::forward<Variant>(v))), true) : false) || ...))
				throw std::bad_variant_access{};

			return ret;
		}
		else
		{
			if (!((v.index() == N ? (std::invoke(std::forward<F>(f), std::get<N>(std::forward<Variant>(v))), true) : false) || ...))
				throw std::bad_variant_access{};
		}
	}


	namespace ext {
		template<typename VariantType, typename T, std::size_t index = 0>
		constexpr std::size_t variant_index() {
			if constexpr (index == std::variant_size_v<VariantType>) {
				return index;
			}
			else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
				return index;
			}
			else {
				return variant_index<VariantType, T, index + 1>();
			}
		}

		template <class F, class Variant>
		decltype(auto) visit(F&& f, Variant&& var)
		{
			return invoke_active(std::forward<F>(f), std::forward<Variant>(var)
				, std::make_index_sequence<std::variant_size_v<std::remove_reference_t<Variant>>>{});
		}
	}



}