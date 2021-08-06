/**
 * @file main.cpp
 * @author Esteban Carrillo (esteban.carrillo@sansano.usm.cl), 201773032-3
 * @brief 
 * @version 0.1
 * @date 2021-07-28
 * 
 * @note Greddy + Simulated Annealing + Best Improvement
 * 
 */

#include "../include/solver.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <cmath>
#include <climits>
#include <chrono>

#define TEMP_LIMIT 500.0
#define INITAL_TEMP 2000.0
#define ITER 2500
#define ALPHA 0.95
#define PROB 0.2


// Recieves user input and generates a configuration
// for the current problem instance
void SetConfiguration(const char* input_file, Configuration *config) {

	// Read instance data so that it can be stored in the
	// Configuration struct `config`
	std::string file_contents;

	{
		std::string line;

		std::fstream fs(input_file, std::fstream::in);

		while (std::getline(fs, line)) {
			file_contents += line;
			file_contents.push_back('\n');
		}

		fs.close();
	}
	int num_end_pos = 0;
	for (std::size_t i = 0; i < file_contents.length(); i++) {
		if (file_contents[i] == ';') { 
			num_end_pos = i;
			break;
		}
	}

	int teams = std::atoi(file_contents.substr(7, num_end_pos - 7).c_str());
	config->CreateArrays(teams);

	const std::regex re_remove_only (
		R"(\s*nTeams\s*=\s*\d+;[\s\n]*\w+[\s\n]*=[\s\n]*\[[\s\n]*\[[\s\n]*)"
	);
	const std::regex re_one_space (
		R"(\][\s\n]*\[[\s\n]*|[\s\n]*\][\s\n]*\];[\s\n]*(\w+[\s\n]*=[\s\n]*\[[\s\n]*\[[\s\n]*)?|\s\s+)"
	);

	// Transform the file contents into a one-line string
	file_contents = std::regex_replace(
		std::regex_replace(
			std::regex_replace(file_contents, re_one_space, " ", std::regex_constants::match_any),
			re_remove_only,
			"",
			std::regex_constants::match_any
		), 
		re_one_space,
		" ",
		std::regex_constants::match_any
	);
	file_contents.pop_back();

	// Iterate through the newly edited string, first populating the distances
	int row = 0;
	int col = 0;
	std::size_t k = 0;

	for (std::size_t i = 0; i < file_contents.length(); i++) {
		
		// Stop if already checked all values needed
		if (row == teams) {
			row = 0;
			break;
		}
		
		// Process number if space character is found
		if (file_contents[i] == ' ') {
			config->dist[row][col] = std::atoi(file_contents.substr(k, i - k).c_str());

			col++;
			k = i + 1;

			if (col == teams) {
				col = 0;
				row++;
			}
		}
	}

	// Get the rest of the values for opponents
	for (std::size_t i = k; i < file_contents.length(); i++) {

		// Process number if space character is found
		if (file_contents[i] == ' ' || i == file_contents.length() - 1) {
			if (i == file_contents.length() - 1) i++;

			config->oppn[row][col] = std::atoi(file_contents.substr(k, i - k).c_str());

			col++;
			k = i + 1;

			if (col == teams) {
				col = 0;
				row++;
			}
		}
		
	}

}


int main(int argc, char* argv[]) {
	(void) argc;

	/**
	 * Steps:
	 * 		1. Get user input
	 * 		2. Generate necessary objects related to problem instance
	 * 			- q1
	 * 			- q2
	 * 			- max_iter
	 * 			- init_temp
	 * 			- min_temp
	 * 			- temp_decrease_ratio (*)
	 * 			- best_impv_ratio (*)
	 * 			- 
	 * 
	 * 		3. Solve
	 * 		4. Generate output
	 */

	auto time_start = std::chrono::high_resolution_clock::now();
	
	// Configuration object holding problem parameters
	Configuration config = Configuration(
		std::atoi(argv[2]), // q1
		std::atoi(argv[3]), // q2
		std::atoi(argv[4])  // K (a.k.a. max_iter)
	);

	SetConfiguration(argv[1], &config);

	std::size_t n_teams = config.teams;
	std::size_t n_umps = n_teams / 2;

	// Initialize umpires

	Umpire umps[n_umps];

	std::vector<Match> init_solution[n_umps];
	std::vector<Match> best_solution[n_umps];
	std::vector<Match> curr_solution[n_umps];

	for (std::size_t i = 0; i < n_umps; i++) {
		umps[i] = Umpire(i, config.dist, config.teams);

		best_solution[i].reserve(2 * n_teams - 2);
		curr_solution[i].reserve(2 * n_teams - 2);
		init_solution[i].reserve(2 * n_teams - 2);
	}

	GreedyMatchingHeuristic(&config, &umps[0]);
	for (std::size_t i = 0; i < n_umps; i++) {
		init_solution[i] = umps[i].GetPath();
		best_solution[i] = umps[i].GetPath();
	}

	int global_best_total_dist = INT_MAX;
	int local_best_total_dist = INT_MAX;
	int curr_total_dist = INT_MAX;

	double temp = INITAL_TEMP;
	int curr_iter = config.max_iter;

	bool using_random = true;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> uni_dbl_dist(0.0, 1.0);

	while (curr_iter-- > 0) {
		temp = INITAL_TEMP;

		for (std::size_t i = 0; i < n_umps; i++) {
			if (uni_dbl_dist(gen) <= 0.2)
				umps[i].SetMatches(&init_solution[i]);
			else
				umps[i].SetMatches(&best_solution[i]);
		}


		while(temp > TEMP_LIMIT) {
			using_random = temp > 0.5 * TEMP_LIMIT;

			for (int i = 0; i < ITER; i++) {

				// Update the current total distance
				local_best_total_dist = 0;
				for (std::size_t j = 0; j < n_umps; j++)
					local_best_total_dist += umps[j].TotalDistance();


				auto [ u1_index, u2_index, time_slot ] = Umpire::PickExchange(&umps[0], n_umps, using_random, config.q1, config.q2);
				
				// Get difference and compare
				curr_total_dist = 0;
				for (std::size_t j = 0; j < n_umps; j++) {

					if (u1_index == (int)j) {
						curr_total_dist += umps[j].TotalDistanceWithExchange(umps[u2_index], time_slot);
						curr_solution[j] = umps[u1_index].GetPathWithExchange(umps[u2_index], time_slot);
					
					} else if (u2_index == (int)j) {
						curr_total_dist += umps[j].TotalDistanceWithExchange(umps[u1_index], time_slot);
						curr_solution[j] = umps[u2_index].GetPathWithExchange(umps[u1_index], time_slot);
					
					} else {
						curr_total_dist += umps[j].TotalDistance();
						curr_solution[j] = umps[j].GetPath();
					}

				}

				if (curr_total_dist < global_best_total_dist) {
					global_best_total_dist = curr_total_dist;
					std::copy(curr_solution, curr_solution + n_umps, best_solution);

					umps[u1_index].SwapMatch(umps[u2_index], time_slot);


				} else {
					
					if (uni_dbl_dist(gen) < std::exp((global_best_total_dist - curr_total_dist) / temp)) {

						umps[u1_index].SwapMatch(umps[u2_index], time_slot);

					}

				}
			}

			temp *= ALPHA;
		}

		if (curr_iter % (config.max_iter / 5) == 0)
			std::cout << "iteration " << config.max_iter - curr_iter << "/" << config.max_iter << "\n" << std::flush;
	}


	int total_cost = 0;
	std::string ump_sol = "";
	auto time_end = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = std::chrono::duration<double, std::milli>(time_end - time_start).count() / 1000.0;


	for (std::size_t i = 0; i < n_umps; i++) {
		total_cost += umps[i].TotalDistance();
		ump_sol += umps[i].ToString() + "\n";
	}

	std::cout << "\nCosto/Distancia total de viaje: " << total_cost << "\n" << std::flush;
	std::cout << ump_sol << std::flush;
	std::cout << "Tiempo total de ejecución: " << elapsed_seconds << " [s]\n" << std::flush;

	for (std::size_t i = 0; i < n_umps; i++)
		umps[i].Delete();

	return 0;
}
