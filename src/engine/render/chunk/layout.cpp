#include "layout.h"
#include <tuple>

bool engine::render::operator==(const Layout::AttrSetting & a, const Layout::AttrSetting & b)
{
	return 
		std::tuple(
			a.glAttribName, 
			a.dataType, 
			a.glComponentsPerVertex, 
			a.glStride, 
			a.glOffset, 
			a.glNormalized, 
			a.isIndices
		)
		== 
		std::tuple(
			b.glAttribName, 
			b.dataType, 
			b.glComponentsPerVertex, 
			b.glStride,
			b.glOffset, 
			b.glNormalized, 
			b.isIndices
		);
}

bool engine::render::operator<(const Layout::AttrSetting & a, const Layout::AttrSetting & b)
{
	return
		std::tuple(
			a.glAttribName,
			a.dataType,
			a.glComponentsPerVertex,
			a.glStride,
			a.glOffset,
			a.glNormalized,
			a.isIndices
		)
		<
		std::tuple(
			b.glAttribName,
			b.dataType,
			b.glComponentsPerVertex,
			b.glStride,
			b.glOffset,
			b.glNormalized,
			b.isIndices
		);
}

bool engine::render::operator==(const Layout & a, const Layout & b)
{
	return a._attribs == b._attribs;
}

bool engine::render::operator<(const Layout & a, const Layout & b)
{
	return a._attribs < b._attribs;
}
