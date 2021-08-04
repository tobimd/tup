#pragma once

#include <iostream>
#include <cstdarg>

namespace debug {
	template<typename... T>
	void log(T... args);
} // namespace debug