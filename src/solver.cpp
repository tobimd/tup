#include "../include/structs.h"
#include "../include/solver.h"

#include <iostream>
#include <algorithm>
#include <limits>

constexpr int penalty = 1000000;

void SolveBipartiteGraph(int *costs, int *fbo, int *sbo, const std::size_t n_umps) {
	int best_cost = 0;
	int curr_cost = 0;

	int second_best_cost = 999999999;
	bool sbc_was_set = false;

	// Indices of vector correspond to each umpire's id, and contains the index
	// of the selected match
	int edges[n_umps];

	// Start with the base case of every umpire being orderly matched with a
	// team pair
	for (std::size_t i = 0; i < n_umps; i++) {
		edges[i] = i;

		sbo[i] = fbo[i];
		fbo[i] = i;

		best_cost += costs[i*n_umps + i];
	}

    std::next_permutation(edges, edges + n_umps);

	// While permutations still exists, loop to find the best two minimal costs
	do {
		// Get the total cost of assigning a team to an umpire based on the
		// permutated edges

		curr_cost = 0;
		for (std::size_t i = 0; i < n_umps; i++) {
			curr_cost += costs[i*n_umps + edges[i]];
		}


		if (curr_cost < best_cost) {

			for (std::size_t i = 0; i < n_umps; i++) {
				if (best_cost < second_best_cost || !sbc_was_set)
					sbo[i] = fbo[i];

				fbo[i] = edges[i];
			}

			second_best_cost = best_cost;
			best_cost = curr_cost;
			sbc_was_set = true;
		} else if (curr_cost < second_best_cost || !sbc_was_set) {

			for (std::size_t i = 0; i < n_umps; i++) {
				sbo[i] = edges[i];
			}
			second_best_cost = curr_cost;
			sbc_was_set = true;
		} 
	} while (std::next_permutation(edges, edges + n_umps));
}

void GreedyMatchingHeuristic(Configuration *config, Umpire *umps) {
	std::size_t n_teams = config->teams;
	std::size_t n_umps = n_teams / 2;
	std::size_t time_slot = 0;
	std::size_t total_slots = 2 * n_teams - 2;

	// The first and second best option obtained from the
	// bipartite graph Perfect Matching

	// When adding matches to each umpire, it's always done using this array,
	// even when choosing the next best option (`second_best_option`). The way
	// it's done for those cases, is replacing the values of this array with the
	// onces from the `second_best_option`.
	int first_best_option[n_umps];

	// Stores the fallback option and it's copied to the previous best `prev_sbo`
	// when concluding a slot loop.
	int second_best_option[n_umps];

	// Second best option from the previous set, used if backtracking to avoid
	// recalculating.
	int prev_sbo[n_umps];

	// Costs of all possible matches per umpire
	int assignment_costs[n_umps][n_umps];

	bool has_violations[n_umps][n_umps];

	bool has_backtracked[total_slots];
	for (std::size_t i = 0; i < total_slots; i++)
		has_backtracked[i] = false;

	int n_distance;
	int n_visits;
	int n_home_violation;
	int n_team_violation;

	Umpire *curr_ump;
	Match *curr_match;

	std::vector<Match> curr_matches;
	curr_matches.reserve(n_umps);

	Match::GetMatches(curr_matches, config->oppn[0], n_teams);

	for (std::size_t i = 0; i < n_umps; i++) {
		umps[i].AddToPath(curr_matches[i]);

		prev_sbo[i] = i;
	}

	// Whether a set of edges or only one breaks a rule 
	bool is_unfeasible;

	// Used to avoid calculating again for the current loop,
	// because it's from backtracking and values are already set
	bool skip_calculations = false;

	time_slot++;

	while (time_slot < total_slots)	{
		Match::GetMatches(curr_matches, config->oppn[time_slot], n_teams);

		if (!skip_calculations) { 

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
		}

		// Before solving the bipartite graph, check if there is
		// no feasible solution for any umpire, and backtrack
		if (time_slot > 1 && !has_backtracked[time_slot] && !skip_calculations) {
			for (std::size_t i = 0; i < n_umps; i++) {
				is_unfeasible = has_violations[i][0];

				if (!is_unfeasible) continue;

				for (std::size_t j = 1; j < n_umps; j++) {
					is_unfeasible = is_unfeasible && has_violations[i][j];
				}

				if (is_unfeasible) {
					for (std::size_t k = 0; k < n_umps; k++) {
						umps[k].Backtrack();
					}

					has_backtracked[time_slot] = true;
					break;
				}
			}

			if (has_backtracked[time_slot]) {
				time_slot--;
				skip_calculations = true;
				std::copy(prev_sbo, prev_sbo + n_umps, first_best_option);
				std::cout << "\n        <---- backtracking (pre perfect matching) <----\n" << std::flush;
				continue;
			}
		}

		if ( !skip_calculations ) { 
			// Solve Perfect Matching on the bipartite graph
			SolveBipartiteGraph(
				&assignment_costs[0][0],
				&first_best_option[0],
				&second_best_option[0],
				n_umps
			);
		}

		// Only check if backtrack is needed if the current slot
		// is grater than 1, or hasn't backtracked already. Same as before
		if (time_slot > 1 && !has_backtracked[time_slot] && !skip_calculations) {
			for (std::size_t i = 0; i < n_umps; i++) {
				if (has_violations[i][first_best_option[i]]) {
					for (std::size_t k = 0; k < n_umps; k++) {
						umps[k].Backtrack();
					}

					has_backtracked[time_slot] = true;
					break;
				}
			}

			if (has_backtracked[time_slot]) {
				time_slot--;
				skip_calculations = true;
				std::copy(prev_sbo, prev_sbo + n_umps, first_best_option);
				std::cout << "\n        <---- backtracking (post perfect matching)<----\n" << std::flush;
				continue;
			}
		}

		std::cout << "\n\n[time_slot=" << time_slot << "], adding matches=" << Array2D::ToString(&first_best_option[0], 1, n_umps)<< "\nwhich is the same as:\n";

		// Now that backtracking and costs have been settled, 
		// add new matches as the path for each umpire
		for (std::size_t i = 0; i < n_umps; i++) {
			std::cout << "    ump[" << i << "] has match: " << curr_matches[first_best_option[i]].ToString() << "\n" << std::flush;
			umps[i].AddToPath(curr_matches[first_best_option[i]]);
		}
		
		std::copy(second_best_option, second_best_option + n_umps, prev_sbo);
		skip_calculations = false;
		time_slot++;
	}

	std::cout << "\n----------------------------\n-- Showing final results: --\n\n" << std::flush;
	for (std::size_t i = 0; i < n_umps; i++) {
		std::cout << "   * " << umps[i].ToString() << "\n" << std::flush;
	}

	std::cout << "----------------------------\n\n" << std::flush;
}

void LocalSearch(Configuration *config) {

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