#pragma once

#include "src/basic_types.h"

namespace engine::render
{
	class DataType {
	public:
		enum Type {
			F32, F64, U8, U16, U32, I8, I16, I32
		};
		DataType(Type v);
		i32 bytesPerElem() const;
		Type value();
		i32 toInt();

		friend bool operator==(const DataType& a, const DataType& b);
		friend bool operator<(const DataType& a, const DataType& b);

		template <typename T>
		static DataType fromT();
	private:
		Type _v;
	};
}