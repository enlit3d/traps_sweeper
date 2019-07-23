#pragma once

#include <vector>
#include "src/basic_types.h"
#include "../opengl/defines/gl_types.h"
#include "src/string_type.h"

namespace engine::render
{
	class Layout {
	public:
		struct AttrSetting {
			AttrSetting(str_ref name, DataType type, i32 componentsPerVertex, bool normalized=false, i32 stride=0, i32 offset=0) 
				: glAttribName(name), dataType(type), glComponentsPerVertex(componentsPerVertex), glStride(stride), glOffset(offset), glNormalized(normalized)
			{}

			i32 bytesPerVertex() const {
				return dataType.bytesPerElem() * glComponentsPerVertex;
			}
		public:
			str_t glAttribName = "";
			DataType dataType = DataType::F32;
			i32 glComponentsPerVertex = 0;
			i32 glStride = 0;
			i32 glOffset = 0;
			bool glNormalized = false;
			bool isIndices = false;

			friend bool operator==(const AttrSetting& a, const AttrSetting& b);
			friend bool operator<(const AttrSetting& a, const AttrSetting& b);
		};
	public:
		Layout() {
			_attribs.emplace_back("", DataType::U16, 1); 
			_attribs.back().isIndices = true;
		}

		Layout& attrPointer(
			str_ref attribName, 
			DataType dataType = DataType::F32,
			int componentsPerVertex = 4,
			bool normalized = false, 
			i32 offset = 0, 
			i32 stride = 0) 
		{
			_attribs.emplace_back(attribName, dataType, componentsPerVertex, normalized, offset, stride);
			return *this;
		}

		using Index = i32;
		Index toIndex(str_ref name) const {
			for (auto i = 0; i < _attribs.size(); ++i) {
				if (_attribs[i].glAttribName == name) {
					return i;
				}
			}
			throw;
		}

		const AttrSetting& operator[](Index index) const {
			return _attribs[index];
		}

		const AttrSetting& operator[](str_ref name) const {
			return this->operator[](toIndex(name));
		}

		auto begin() const {return _attribs.begin();}
		auto end() const { return _attribs.end(); }

		friend bool operator==(const Layout& a, const Layout& b);
		friend bool operator<(const Layout& a, const Layout& b);
	private:
		std::vector<AttrSetting> _attribs = {};
	};
	

}