#include "gl_types.h"

namespace engine::render
{
	DataType::DataType(Type v) : _v(v)
	{}
	i32 DataType::bytesPerElem() const
	{
		switch (_v) {
		case F32:
			return sizeof(f32);
		case F64:
			return sizeof(f64);
		case U8:
			return sizeof(u8);
		case U16:
			return sizeof(u16);
		case U32:
			return sizeof(u32);
		case I8:
			return sizeof(i8);
		case I16:
			return sizeof(i16);
		case I32:
			return sizeof(i32);
		default:
			throw;
		}
	}
	DataType::Type DataType::value()
	{
		return _v;
	}
	i32 DataType::toInt() {
		return static_cast<i32>(_v);
	}
	bool operator==(const DataType & a, const DataType & b)
	{
		return a._v == b._v;
	}
	bool operator<(const DataType & a, const DataType & b)
	{
		return a._v < b._v;
	}

	template<>
	DataType DataType::fromT<f32>() {
		return DataType{ Type::F32 };
	};

	template<>
	DataType DataType::fromT<u8>() {
		return DataType{ Type::U8 };
	};
}