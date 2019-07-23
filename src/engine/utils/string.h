#pragma once
#include "src/string_type.h"
#include <vector>

namespace engine::string
{
	str_t join(const std::vector<str_t>& v, str_ref sap);
	std::vector<str_t> split(str_ref string, str_ref sap);
	bool startsWith(str_ref string, str_ref start);
}