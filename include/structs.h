#pragma once

#include <string>      //
#include <cstdlib>     // -- atoi
#include <vector>

struct Array2D {
	int **array;

	std::size_t rows;
	std::size_t cols;

	Array2D() = default;
	Array2D(const int __rows, const int __cols);
	Array2D(const Array2D& other);

	Array2D &operator=(const Array2D &other);

	void Delete();

	std::string ToString();
	int* operator[](const int index);
	int* operator[](const size_t index);

	template<class T>
	static std::string ToString(T *p_array, const std::size_t rows, const std::size_t cols) {
		std::string string = "\n[";

		for (std::size_t i = 0; i < rows; i++) {
			if (i != 0) {
				string += " ";
			}

			string += "[";

			for (std::size_t j = 0; j < cols; j++) {
				string += " " + std::to_string(p_array[1*i + j]);
			}

			string += " ]";

			if (i + 1 != rows) {
				string += "\n";
			}
		}
		string += "]\n";

		return string;
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

	Configuration(const int q1, const int q2, const int max_iter);
	~Configuration();

	void CreateArrays(const int __teams);
};

struct Match {
	int home;
	int visit;

	Match() = default;
	Match(int home, int visit);

	std::string ToString();

	static void GetMatches(std::vector<Match> &matches, Configuration *config, const int time_slot) {
		matches.clear();

		// Number of matches
		int n_size = config->teams / 2;

		// Iterate through opponents list, and create each match accordingly
		for (int j = 0; j < n_size; j++) {

			// If a positive value is found, then set `home` team as the current
			// `j` index + 1, and `visit` team as the value being hold in that position (positive)
			if (config->oppn[time_slot][j] > 0)	{
				matches.emplace(matches.end(), Match(j+1, -config->oppn[time_slot][j]));
			}
		}
	}

};

class Umpire {

private:
	int n_length_ = 0;
	int n_size_;

public:
	int id;

	Match *p_path;

	Array2D dist;

	// Locked home venue index, due to backtracking
	int locked;

	Umpire() = default;
	Umpire(const int id, Array2D dist, const int n_teams);

	void Delete();

	void AddToPath(const Match match);
	void Backtrack();
	int DistanceTo(const int home_venue);
	int CountVisitsOf(const int home_venue);
	int HomeVisitViolations(const int home_venue, const int q1);
	int TeamVisitViolations(const int q2);
	bool HasVisited(const int home_venue);
};

struct Instance {
	
};