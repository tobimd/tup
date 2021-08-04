#include "../include/structs.h"
#include "../include/solver.h"
#include "../include/utils.h"

#include <iostream>

constexpr int penalty = 1000000;

void GreedyMatchingHeuristic(Configuration *config) {
	std::size_t n_teams = config->teams;
	std::size_t n_umps = n_teams / 2;
	std::size_t time_slot = 0;
	std::size_t total_slots = 2 * n_teams - 2;

	// Costs of all possible matches per umpire
	int assignment_costs[n_umps][n_umps];
	bool has_violations[n_umps][n_umps];

	int n_distance;
	int n_visits;
	int n_home_violation;
	int n_team_violation;

	Umpire *curr_ump;
	Match *curr_match;

	std::vector<Match> curr_matches;
	curr_matches.reserve(n_umps);

	Match::GetMatches(curr_matches, config->oppn[0], n_teams);
	time_slot++;

	Umpire umps[n_umps];
	for (std::size_t i = 0; i < n_umps; i++) {
		umps[i] = Umpire(i, config->dist, n_teams);
		umps[i].AddToPath(curr_matches[i]);
	}

	while (time_slot < total_slots)	{
		Match::GetMatches(curr_matches, config->oppn[time_slot], n_teams);

		// Calculate costs for each edge, for each umpire
		for (std::size_t ump_index = 0; ump_index < n_umps; ump_index++) {
			for (std::size_t match_index = 0; match_index < n_umps; match_index++) {	
				curr_ump = &umps[ump_index];
				curr_match = &curr_matches[match_index];

				n_distance = curr_ump->DistanceTo(curr_match->home);
				n_visits = curr_ump->CountVisitsOf(curr_match->home);

				n_home_violation = curr_ump->HomeVisitViolations(curr_match->home, config->q1);
				n_team_violation = curr_ump->TeamVisitViolations(config->q2);

				assignment_costs[ump_index][match_index] = n_distance + penalty * (n_visits + n_home_violation + n_team_violation);
				has_violations[ump_index][match_index] = n_home_violation + n_team_violation > 0;
			}
		}

		for (std::size_t i = 0; i < n_umps; i++) {
			umps[i].AddToPath(curr_matches[i]);
		}

		time_slot++;
	}

	for (std::size_t i = 0; i < n_umps; i++) {
		umps[i].Delete();
	}
}

void LocalSearch() {

}

void SimulatedAnnealing(Configuration *config) {
	/**
	while time limit and iteration limit not exceeded; do

		S = initial solution with prob. p or incumbent solution with prob. (1 − p)
		t = t0

	    while t > TEMP_LIMIT do
			for all ITER iterations do
				Pick one feasible exchange E at random
				d = impact of E in objective function

				if d < 0; then
					Execute E
					if new solution better than incumbent; then
						Update incumbent
					end if

				else
					x = random number in [0, 1]
					if x < exp(−d/t); then
						Execute E
					end if
				end if
			end for			

			t = t ∗ ALPHA

		end while
	end while */
}