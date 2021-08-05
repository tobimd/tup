#include "../../include/structs.h"

Match::Match(int home, int visit) : home(home), visit(visit) {}

std::string Match::ToString() {
	return "<Match: home=" + std::to_string(home) + ", visit=" + std::to_string(visit) + ">";
}