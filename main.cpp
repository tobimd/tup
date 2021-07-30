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


// Generates a configuration for the complete problem
// instance.
struct Configuration {
	int q1;
	int q2;
	int teams;
	int **dist;
	int **oppn;

	Configuration(int q1, int q2, int teams, int **dist, int **oppn) {
		Configuration::q1 = q1;
		Configuration::q2 = q2;
		Configuration::teams = teams;
		Configuration::dist = dist;
		Configuration::oppn = oppn;
	}

	~Configuration() {
		for (int i = 0; i < teams; i++) {
			delete[] dist[i];
			delete[] oppn[i];
		}

		delete[] dist;
		delete[] oppn;
	}

	int DistArraySize() {
		return teams * teams;
	}

	int OpponentsArraySize() {
		return 2 * teams * teams - 2 * teams;
	}

};

// Recieves user input and generates a configuration
// for the current problem instance
Configuration GetProblemParameters(char* argv[]) {

	// Read instance data so that it can be stored in the
	// Configuration struct `config`
	std::string file_contents;

	{
		std::string line;

		std::fstream fs(argv[1], std::fstream::in);

		while (std::getline(fs, line)) {
			file_contents += line;
			file_contents.push_back('\n');
		}

		fs.close();
	}
	int num_end_pos = 0;
	for (int i = 0; i < file_contents.length(); i++) {
		if (file_contents[i] == ';') { 
			num_end_pos = i;
			break;
		}
	}

	int teams = std::atoi(file_contents.substr(7, num_end_pos - 7).c_str());
	int **dist = new int*[teams];
	int **oppn = new int*[2*teams - 2];

	for (int i = 0; i < teams; i++) {
		dist[i] = new int[teams];
	}

	for (int i = 0; i < 2*teams - 2; i++) {
		oppn[i] = new int[teams];
	}

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
	int k = 0;

	for (int i = 0; i < file_contents.length(); i++) {
		
		// Stop if already checked all values needed
		if (row == teams) {
			row = 0;
			break;
		}
		
		// Process number if space character is found
		if (file_contents[i] == ' ') {
			dist[row][col] = std::atoi(file_contents.substr(k, i - k).c_str());				

			col++;
			k = i + 1;

			if (col == teams) {
				col = 0;
				row++;
			}
		}
	}

	// Get the rest of the values for opponents
	for (int i = k; i < file_contents.length(); i++) {

		// Process number if space character is found
		if (file_contents[i] == ' ' || i == file_contents.length() - 1) {
			if (i == file_contents.length() - 1) i++;

			oppn[row][col] = std::atoi(file_contents.substr(k, i - k).c_str());

			col++;
			k = i + 1;

			if (col == teams) {
				col = 0;
				row++;
			}
		}
		
	}

/* 	std::cout << "data: \"" << file_contents << "\"\n\nnTeams=" << teams << ";\n\ndist=\n";

	for (int i = 0; i < teams; i++) {
		for (int j = 0; j < teams; j++) {
			std::cout << " " << dist[i][j];
		}
		std::cout << std::endl; 
	}
	std::cout << "\n\nopponents=\n";

	for (int i = 0; i < 2 * teams - 2; i++) {
		for (int j = 0; j < teams; j++) {
			std::cout << " " << oppn[i][j];
		}
		std::cout << std::endl; 
	}
 */
	return Configuration(std::atoi(argv[2]), std::atoi(argv[3]), teams, dist, oppn);
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
	
	Configuration config = GetProblemParameters(argv);

	return 0;
}