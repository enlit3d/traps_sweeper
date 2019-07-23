#include "material.h"
#include <tuple>

namespace engine::render {

	namespace mat {
		template <Tag T>
		Flag<T>::Flag(i32 v)
			: _v{ v }
		{}

		template <Tag T>
		bool Flag<T>::operator==(const Flag<T> & other) const
		{
			return _v == other._v;
		}

		template <Tag T>
		bool Flag<T>::operator!=(const Flag<T> & other) const
		{
			return !(*this == other);
		}

		template <Tag T>
		bool Flag<T>::operator<(const Flag<T> & other) const 
		{
			return _v < other._v;
		}

		template class Flag<Tag::TRANSFORM>;
		Flag<Tag::TRANSFORM> TRANSFORM_STD = 0;
		Flag<Tag::TRANSFORM> TRANSFORM_POS_ONLY = 1;

		template class Flag<Tag::MODEL_DATA>;
		Flag<Tag::MODEL_DATA> MODEL_DATA_NONE = 0;
		Flag<Tag::MODEL_DATA> MODEL_DATA_USE = 1;

		template class Flag<Tag::TEXTURE_COUNT>;
		Flag<Tag::TEXTURE_COUNT> TEXTURE_NONE = 0;
		Flag<Tag::TEXTURE_COUNT> TEXTURE_ONE = 1;
	}

	constexpr auto _tie(const MatFlags& m) {
		return std::tie(m.transform, m.modelData, m.textures);
	}
	
	bool MatFlags::operator<(const MatFlags & other) const
	{
		return _tie(*this) < _tie(other);
	}

	bool operator==(const MatFlags & a, const MatFlags & b)
	{
		return _tie(a) == _tie(b);
	}

}