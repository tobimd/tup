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
		R"(nTeams=\d+;[\s\n]*\w+[\s\n]*=[\s\n]*\[[\s\n]*\[[\s\n]*)"
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

	std::cout << "file_contents: \"" << file_contents << "\"\n" << std::flush;

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
	std::cout << "dist: " << config->dist.ToString() << "\n" << std::flush;

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

	std::cout << "oppn: " << config->oppn.ToString() << "\n" << std::flush;

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
	
	// Configuration object holding problem parameters
	Configuration config = Configuration(
		std::atoi(argv[2]), // q1
		std::atoi(argv[3]), // q2
		std::atoi(argv[4])  // K (a.k.a. max_iter)
	);

	SetConfiguration(argv[1], &config);

	std::size_t n_teams = config.teams;
	std::size_t n_umps = n_teams / 2;

	Umpire umps[n_umps];
	for (std::size_t i = 0; i < n_umps; i++)
		umps[i] = Umpire(i, config.dist, config.teams);

	GreedyMatchingHeuristic(&config, &umps[0]);




	for (std::size_t i = 0; i < n_umps; i++)
		umps[i].Delete();

	return 0;
}