#include "console.h"

#include <iostream>

namespace engine::console {
	void out(str_ref output)
	{
		std::cout << output << "\n";
	}
}