#include "../../include/structs.h"

// Generates a configuration for the complete problem
// instance.
Configuration::Configuration(const int q1, const int q2, const int max_iter) {
	Configuration::q1 = q1;
	Configuration::q2 = q2;
	Configuration::max_iter = max_iter;
}

void Configuration::CreateArrays(const int __teams) {
	teams = __teams;

	dist = Array2D(teams, teams);
	oppn = Array2D(2*teams - 2, teams);
}