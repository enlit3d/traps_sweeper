#include "string.h"

namespace engine::string
{
	str_t join(const std::vector<str_t>& v, str_ref sap)
	{
		if (v.empty()) { return ""; }
		// calculate size of result string
		auto size = (v.size() - 1) * sap.size();
		for (const auto& s : v) {
			size += s.size();
		}

		str_t str = "";
		str.reserve(size);
		for (auto i = 0; i < v.size() - 1; i++) { // append last elem later
			str += v[i];
			str += sap;
		}
		str += v.back();
		//assert(str.size() == size);
		return str;
	}
	std::vector<str_t> split(str_ref string, str_ref sap)
	{
		std::vector<std::string> v;
		if (!string.empty()) {
			std::string::size_type start = 0;
			while (true) {
				auto x = string.find(sap, start);
				if (x == std::string::npos) {
					break;
				}

				auto len = x - start;
				v.emplace_back(string.substr(start, len));
				start += len + sap.size();
			}

			v.emplace_back(string.substr(start));
		}
		return v;
	}
	bool startsWith(str_ref string, str_ref start)
	{
		return start.size() <= string.size() && string.compare(0, start.size(), start) == 0;
	}
}