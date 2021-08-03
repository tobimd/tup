#include "structs.h"

struct Array2D {
	int **array;

	std::size_t rows;
	std::size_t cols;

	Array2D() = default;
	Array2D(const int __rows, const int __cols) : rows(__rows), cols(__cols) {
		array = new int*[rows];

		for (std::size_t i = 0; i < rows; i++) {
			array[i] = new int[cols];
		}
	}

	// Copy constructor
	Array2D(const Array2D& other) : rows(other.rows), cols(other.cols) {
		array = new int*[rows];

		for (std::size_t i = 0; i < rows; i++) {
			array[i] = new int[cols];
		}

		for (std::size_t i = 0; i < rows; i++) {
			for (std::size_t j = 0; j < cols; j++) {

				array[i][j] = other.array[i][j];

			}
		}
	}

	Array2D &operator=(const Array2D &other) {
		rows = other.rows;
		cols = other.cols;

		array = new int*[rows];

		for (std::size_t i = 0; i < rows; i++) {
			array[i] = new int[cols];
		}

		for (std::size_t i = 0; i < rows; i++) {
			for (std::size_t j = 0; j < cols; j++) {

				array[i][j] = other.array[i][j];

			}
		}

		return *this;
	}

	// Manual call for destructor
	void Delete() {
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

	static std::string ToString(int *p_array, const std::size_t rows, const std::size_t cols) {
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
		dist.Delete();
		oppn.Delete();
	}

	void CreateArrays(const int __teams) {
		teams = __teams;

		dist = Array2D(teams, teams);
		oppn = Array2D(2*teams - 2, teams);
	}

};

struct Match {
	int home;
	int visit;

	Match() = default;
	Match(int home, int visit) : home(home), visit(visit) {}

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

	std::string ToString() {
		return "<Match: home=" + std::to_string(home) + ", visit=" + std::to_string(visit) + ">";
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
	Umpire(const int id, Array2D dist, const int n_teams) : n_size_(n_teams), id(id), dist(dist) { 
		p_path = new Match[n_size_];
	}

	void Delete() {
		delete[] p_path;
	}

	void AddToPath(const Match match) {
		p_path[n_length_] = match;

		++n_length_;
		locked = -1;
	}

	void Backtrack() {
		--n_length_;
		locked = p_path[n_length_].home;
	}

	int DistanceTo(const int home_venue) {
		return dist[p_path[n_length_ - 1].home][home_venue];
	}

	int CountVisitsOf(const int home_venue) {
		int count = 0;

		for (int i = 0; i < n_length_; i++) {
			if (p_path[i].home == home_venue)
				count++;
		}

		return count;
	}

	int HomeVisitViolations(const int home_venue, const int q1) {
		int count = 0;
		int start = n_length_ - 1;

		for (int i = start; i > start - q1; i--)
			if (p_path[i].home == home_venue) count++;

		return count;
	}

	int TeamVisitViolations(const int q2) {
		int count = 0;
		int team_visits[n_size_];
		int start = n_length_ - 1;

		for (int i = 0; i < n_size_; i++)
			team_visits[i] = 0;

		for (int i = start; i > start - q2; i--){ 
			team_visits[p_path[i].home]++;
			team_visits[p_path[i].visit]++;
		}

		for (int i = 0; i < n_size_; i++)
			if (team_visits[i] > 0) count++;

		return count;
	}

	bool HasVisited(const int home_venue) {
		for (int i = 0; i < n_length_; i++) {
			if (p_path[i].home == home_venue) {
				return true;
			}
		}

		return false;
	}
};

struct Instance {
	
};