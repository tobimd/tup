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
#include <vector>
#include <limits>

enum Param { Dist, Oppn };

struct Array2D {
	int **array;

	std::size_t rows;
	std::size_t cols;

	Array2D() = default;
	Array2D(const int __rows, const int __cols) {
		rows = __rows;
		cols = __cols;

		array = new int*[rows];

		for (std::size_t i = 0; i < rows; i++) {
			array[i] = new int[cols];
		}
	}

	// Manual call for destructor
	void deleteAll() {
		for (std::size_t i = 0; i < rows; i++) {
			delete[] array[i];
		}

		delete[] array;
	}

	std::string ToString() {
		std::string string = "\n[";

		for (std::size_t i = 0; i < rows; i++) {
			if (i != 0) {
				string += " ";
			}

			string += "[";

			for (std::size_t j = 0; j < cols; j++) {
				string += " " + std::to_string(array[i][j]);
			}

			string += " ]";

			if (i + 1 != rows) {
				string += "\n";
			}
		}
		string += "]\n";

		return string;
	}

	int* operator[](const int index) {
		return array[index];
	}

	int* operator[](const size_t index) {
		return array[index];
	}
};

// Generates a configuration for the complete problem
// instance.
struct Configuration {
	int q1;
	int q2;
	int max_iter;

	int teams;
	Array2D dist;
	Array2D oppn;

	Configuration(const int q1, const int q2, const int max_iter) {
		Configuration::q1 = q1;
		Configuration::q2 = q2;
		Configuration::max_iter = max_iter;
	}

	~Configuration() {
		dist.deleteAll();
		oppn.deleteAll();
	}

	void CreateArrays(const int __teams) {
		teams = __teams;

		dist = Array2D(teams, teams);
		oppn = Array2D(2*teams - 2, teams);
	}

};

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
}

struct Match {
	int ump = -1;
	int home;
	int visit;

	double distance = 0.0;

	Match() = default;
	Match(int home, int visit) : home(home), visit(visit) {}

	std::string ToString() {
		return "<Match: home=" + std::to_string(home) + ", visit=" + std::to_string(visit) + ", ump=" + std::to_string(ump) + ">";
	}
};

struct Slot {

	// List of size 2 * teams - 2, consisting of each match between two teams
	std::vector<Match> matches;

	Slot() = default;
	Slot(Configuration *config, const int time_slot) {
		// Number of matches
		int n_size = config->teams / 2;

		matches.reserve(n_size); // Reserve known ammount of memory

		// Iterate through opponents list, and create each match accordingly
		for (int j = 0; j < n_size; j++) {

			// If a positive value is found, then set `home` team as the current
			// `j` index, and `visit` team as the value being hold in that position
			if (config->oppn[time_slot][j] > 0)	{
				matches.emplace(matches.end(), Match(j, config->oppn[time_slot][j]));
			}
		}
	}

	bool operator==(const Slot &other) {
		if (matches.size() != other.matches.size()) return false;

		for (std::size_t i = 0; i < matches.size(); i++) {
			
			if (matches[i].home != other.matches[i].home
				|| matches[i].visit != other.matches[i].visit
				|| matches[i].ump != other.matches[i].ump)	{

				return false;
			}
		}

		return true;
	}

	bool operator!=(const Slot &other) {
		if (matches.size() != other.matches.size()) return true;

		for (std::size_t i = 0; i < matches.size(); i++) {
			
			if (matches[i].home != other.matches[i].home
				|| matches[i].visit != other.matches[i].visit
				|| matches[i].ump != other.matches[i].ump)	{

				return true;
			}
		}

		return false;
	}

	std::string ToString() {
		std::string string = "[" + matches[0].ToString();

		for (std::size_t i = 1; i < matches.size(); i++) {
			string += ", " + matches[i].ToString();
		}

		return string + "]";
	}

	void SetUmpires(int umps[]) {
		for (int i = 0; i < matches.size(); i++) {
			matches[i].ump = umps[i];
		}
	}
};

struct Instance {
	std::vector<Slot> slots;
};

void GreedyMatchingHeuristic(Configuration *config) {
	struct SolutionTree {
		bool locked = false;

		int time_slot;

		std::vector<SolutionTree> nodes;

		Slot slot;

		SolutionTree(int __level, Configuration *config) {
			slot = Slot(config, __level);
			time_slot = __level;
		}
	};

	std::size_t n_teams = config->teams;
	std::size_t n_umps = n_teams / 2;
	std::size_t n_costs = n_umps * n_umps;

	int first_umps[n_umps];
	for (int i = 0; i < n_umps; i++) {
		first_umps[i] = i;

	}

	// Set root of the search tree

	SolutionTree root = SolutionTree(0, config);
	root.slot.SetUmpires(first_umps);
	root.nodes.emplace(root.nodes.end(), SolutionTree(1, config));

	SolutionTree *current = &root;
	SolutionTree *next = &root.nodes[0];

	// Costs of all possible matches per umpire
	double assignment_costs[n_costs];
	double prev_costs[n_costs];
	for (std::size_t i = 0; i < n_costs; i++) {
		assignment_costs[i] = std::numeric_limits<double>::infinity();
		prev_costs[i] = std::numeric_limits<double>::infinity();
	}

	const double penalty = 10000;
	double distance;
	double incentive;
	int violations;

	Match curr_match;

	int time_slots = 2 * config->teams - 2;
	while (current->time_slot < time_slots) {

		// Calculate costs for each edge, for each umpire
		for (std::size_t ump = 0; ump < n_umps; ump++) {
			for (std::size_t match = 0; match < n_umps; match++) {
				curr_match = next->slot.matches[match];

				distance = double(config->dist[curr_match.home][curr_match.visit]);


			}
		}
		
	}
}

void LocalSearch() {

}

void SimulatedAnnealing() {
	
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
	GreedyMatchingHeuristic(&config);

	return 0;
}