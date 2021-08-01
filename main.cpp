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

#include <iostream>    //
#include <string>      //
#include <fstream>     // -- fstream
#include <cstdlib>     // -- atoi
#include <regex>       // -- regex_replace
#include <ctime>       // 

enum Param { Dist, Oppn };

// Generates a configuration for the complete problem
// instance.
struct Configuration {
	int q1;
	int q2;
	int max_iter;

	int teams;
	int **dist;
	int **oppn;

	std::pair<std::size_t, std::size_t> distShape;
	std::pair<std::size_t, std::size_t> oppnShape;

	Configuration(int q1, int q2, int max_iter) {
		Configuration::q1 = q1;
		Configuration::q2 = q2;
		Configuration::max_iter = max_iter;
	}

	~Configuration() {
		for (std::size_t i = 0; i < distShape.first; i++) {
			delete[] dist[i];
		}

		for (std::size_t i = 0; i < oppnShape.first; i++) {
			delete[] oppn[i];
		}

		delete[] dist;
		delete[] oppn;
	}

	void CreateArrays(int __teams) {
		teams = __teams;

		distShape = std::pair<std::size_t, std::size_t>(teams, teams);
		oppnShape = std::pair<std::size_t, std::size_t>(2*teams-2, teams);

		dist = new int*[distShape.first];
		oppn = new int*[oppnShape.first];

		for (std::size_t i = 0; i < distShape.first; i++) {
			dist[i] = new int[distShape.second];
		}

		for (std::size_t i = 0; i < oppnShape.first; i++) {
			oppn[i] = new int[oppnShape.second];
		}
	}

	std::string ToString(Param param) {
		if (param == Param::Dist) {
			return ToString(dist, distShape);

		} else {
			return ToString(oppn, oppnShape);

		}

	}

	std::string ToString(int **array, std::pair<size_t, size_t> shape) {
		std::string string = "\n[";

		for (std::size_t i = 0; i < shape.first; i++) {
			if (i != 0) {
				string += " ";
			}

			string += "[";

			for (std::size_t j = 0; j < shape.second; j++) {
				string += " " + std::to_string(array[i][j]);
			}

			string += " ]";

			if (i + 1 != shape.first) {
				string += "\n";
			}
		}
		string += "]\n";

		return string;
	}
};

// Recieves user input and generates a configuration
// for the current problem instance
void SetProblemParameters(char* input_file, Configuration *config) {

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

void SimulatedAnnealingBestImprovement(Configuration *config) {
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


	double temp_0 = 2000.0;
	double TEMP_LIST = 500.0;
	int N_ITER = config->max_iter;
	double ALPHA = 0.95;
	double p = 0.2;

	int curr_iter = 0;

	while (curr_iter < N_ITER) {
		
	}
}


int main(int argc, char* argv[]) {
	(void) argc;

	/**
	 * Steps:
	 * 		1. Get user input
	 * 		2. Generate necessary objects related to problem instance
	 * 		3. Solve
	 * 		4. Generate output
	 */
	
	// Configuration object holding problem parameters
	Configuration config = Configuration(
		std::atoi(argv[2]), // q1
		std::atoi(argv[3]), // q2
		std::atoi(argv[4])  // K (a.k.a. max_iter)
	);

	SetProblemParameters(argv[1], &config);
	SimulatedAnnealingBestImprovement(&config);

	return 0;
}