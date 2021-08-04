#include "../include/utils.h"

namespace debug
{
	template<typename... T>
	void log(T... args) {
		std::string output = "[DEBUG::LOG] ";

		((output += std::string(args)), ...);

		std::cout << output << "\n" << std::flush;
	}

} // namespace debug
