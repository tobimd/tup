#pragma once

#include <iostream>
#include <string>      //
#include <vector>
#include <tuple>
#include <algorithm>
#include <random>

struct Match {
	int home;
	int visit;

	Match() = default;
	Match(int home, int visit);

	std::string ToString();

	static void GetMatches(std::vector<Match> &matches, int *oppn, const std::size_t size) {
		int j = 0;
		for (std::size_t i = 0; i < size; i++) {
			if (oppn[i] > 0) {
				matches[j] = Match(i+1, oppn[i]);
				j++;
			}
		}
	}

};

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
	static std::string ToString(T **p_array, const std::size_t rows, const std::size_t cols) {
		std::string string = "[";

		for (std::size_t i = 0; i < rows; i++) {
			if (i != 0) {
				string += " ";
			}

			string += "[";

			for (std::size_t j = 0; j < cols; j++) {
				string += " " + std::to_string(p_array[i][j]);
			}

			string += " ]";

			if (i + 1 != rows) {
				string += "\n";
			}
		}
		string += "]";

		return string;
	}

	static std::string ToString(Match *p_array, const std::size_t rows, const std::size_t cols) {
		std::string string = "[";

		for (std::size_t i = 0; i < rows; i++) {
			if (i != 0) {
				string += " ";
			}

			string += "[";

			for (std::size_t j = 0; j < cols; j++) {
				string += " " + std::to_string(p_array[i*cols + j].home);
			}

			string += " ]";

			if (i + 1 != rows) {
				string += "\n";
			}
		}
		string += "]";

		return string;
	}

	template<class T>
	static std::string ToString(T *p_array, const std::size_t rows, const std::size_t cols) {
		std::string string = "[";

		for (std::size_t i = 0; i < rows; i++) {
			if (i != 0) {
				string += " ";
			}

			string += "[";

			for (std::size_t j = 0; j < cols; j++) {
				string += " " + std::to_string(p_array[i*cols + j]);
			}

			string += " ]";

			if (i + 1 != rows) {
				string += "\n";
			}
		}
		string += "]";

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
	~Configuration() {
		dist.Delete();
		oppn.Delete();
	}

	void CreateArrays(const int __teams);
};

int clamp(int value, const int min, const int max);
class Umpire {

private:
	int n_length_ = 0;
	int n_size_;
	int n_teams_;

public:
	int id;

	Match *p_path;

	Array2D dist;

	Umpire() = default;
	Umpire(const int id, Array2D dist, const int n_teams);

	Umpire(const Umpire& other) = default;
	Umpire(Umpire&& other) = default;
	
	Umpire& operator=(Umpire& other) = default;
	Umpire& operator=(Umpire&& other);

	void Delete();
	
	void SetMatches(const std::vector<Match> *matches);
	static Umpire FromMatches(const std::vector<Match> &matches, const int id, Array2D dist, const int n_teams) {
		Umpire u = Umpire(id, dist, n_teams);

		for (int i = 0; i < n_teams; i++) {
			u.AddToPath(matches[i]);
		}

		return u;
	}

	void AddToPath(const Match match);
	void Backtrack();

	std::vector<Match> GetPath();
	std::vector<Match> GetPathWithExchange(Umpire &other, const int time_slot);

	int DistanceTo(const int home_venue);
	int CountVisitsOf(const int home_venue);
	int HomeVisitViolations(const int home_venue, const int q1);
	int TeamVisitViolations(const int q2);
	int TotalDistance();
	int TotalDistanceWithExchange(Umpire &other, const int time_slot);
	void SwapMatch(Umpire &other, const std::size_t time_slot);

	static bool SwappingHasViolations(Umpire *ump1, Umpire *ump2, const std::size_t time_slot, const int n_teams, const int q1, const int q2) {
		int length = ump1->n_length_;

		int q1_check_start = clamp(time_slot + 1 - q1, 0, length);
		int q2_check_start = clamp(time_slot + 1 - q2, 0, length);

		int q1_check_end = clamp(time_slot - 1 + q1, 0, length);
		int q2_check_end = clamp(time_slot - 1 + q2, 0, length);

		int home_visits1[n_teams];
		int team_visits1[n_teams];
		int home_visits2[n_teams];
		int team_visits2[n_teams];

		for (int i = 0; i < n_teams; i++) {
			home_visits1[i] = 0;
			team_visits1[i] = 0;
			home_visits2[i] = 0;
			team_visits2[i] = 0;
		}


		// Check for home visit violations
		for (int i = q1_check_start; i < q1_check_end; i++) {
			for (int j = 0; j < q1; j++) {

				// Use swapped matches
				if (i + j == (int)time_slot) {
					home_visits1[ump2->p_path[i+j].home - 1]++;
					home_visits2[ump1->p_path[i+j].home - 1]++;
				} else {
					home_visits1[ump1->p_path[i+j].home - 1]++;
					home_visits2[ump2->p_path[i+j].home - 1]++;
				}
				
			}

			// Check if violation ocurrs, otherwise reset array
			for (int k = 0; k < n_teams; k++) {
				if (home_visits1[k] > 1 || home_visits2[k] > 1)
					return true;

				home_visits1[k] = 0;				
				home_visits2[k] = 0;
			}

			std::cout << "        (2.2.2)   q1 check\n"  << std::flush;
		}

		// Check for team visit violations
		for (int i = q2_check_start; i < q2_check_end; i++) {
			for (int j = 0; j < q2; j++) {

				std::cout << "        (2.2.2.5) ump2->p_path[" << i << "+" << j << "] = " << ump2->p_path[i+j].ToString() << "\n" << std::flush;

				// Use swapped matches
				if (i + j == (int)time_slot) {
					team_visits1[ump2->p_path[i+j].home - 1]++;
					team_visits1[ump2->p_path[i+j].visit - 1]++;
					team_visits2[ump1->p_path[i+j].home - 1]++;
					team_visits2[ump1->p_path[i+j].visit - 1]++;
				} else {
					team_visits1[ump1->p_path[i+j].home - 1]++;
					team_visits1[ump1->p_path[i+j].visit - 1]++;
					team_visits2[ump2->p_path[i+j].home - 1]++;
					team_visits2[ump2->p_path[i+j].visit - 1]++;
				}

			}

			// Check if violation ocurrs, otherwise reset array
			for (int k = 0; k < n_teams; k++) {
				if (team_visits1[k] > 1 || team_visits2[k] > 1)
					return true;

				team_visits1[k] = 0;				
				team_visits2[k] = 0;				
			}
			std::cout << "        (2.2.3)   q2 check\n"  << std::flush;

		}

		return false;
	}

	// For all umpires in array, return the two indices and the time_slot where
	// a feasible exchange can happen between the two umpires and the corresponding
	// matches
	static std::tuple<int, int, int> PickExchange(Umpire *umps, const std::size_t n_ump_size, const bool use_random, const int q1, const int q2) {

		// The total number of all possible combinations equates to 2*n^3 - 3*n^2 + n
		// using combinatorial n over 2, times the number of matches.

		std::size_t n_slots = 4*n_ump_size - 2;
		std::size_t n_perms = (n_ump_size * (n_ump_size - 1)) / 2; // Binomial `size` over 2

		std::vector<std::tuple<int, int, int>> feasible_perms;
		feasible_perms.reserve(n_slots * n_perms / 2);

		std::tuple<int, int, int> all_perms[n_slots][n_perms];
		
		std::size_t i = 0;
		std::size_t j = 0;

		std::cout << "    ( 2.1 )   inside PickExchange\n" << std::flush;


		for (std::size_t slot = 0; slot < n_slots; slot++) {
			
			i = 0;
			j = 0;
			for (std::size_t perm = 0; perm < n_perms; perm++) {
				
				all_perms[slot][perm] = {i, j, slot};

				std::cout << "    ( 2.2 )   before checking if swapping has violations\n" << std::flush;
				if (!Umpire::SwappingHasViolations(
						&umps[i], &umps[j],	slot,
						n_ump_size * 2, q1, q2)) {

					feasible_perms.emplace_back(i, j, slot);
				}

				j++;

				if (j == n_ump_size) {
					j = 0;
					i++;
				}

				if (i == n_ump_size) {
					i = 0;
					j = 0;
				}
			}
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> uni_int_dist;

		if (feasible_perms.empty()) {
			std::cout << "    ( 2.3 )   feasible perms where empty\n" << std::flush;
			uni_int_dist = std::uniform_int_distribution<int>(0, n_perms - 1);
			int slot_index = clamp(uni_int_dist(gen), 0, n_slots);

			return all_perms[slot_index][uni_int_dist(gen)];

		} else {
			if (use_random && feasible_perms.size() > 1) {
				std::cout << "    ( 2.4 )   using random\n" << std::flush;
				uni_int_dist = std::uniform_int_distribution<int>(0, feasible_perms.size() -1);

				return feasible_perms[uni_int_dist(gen)];

			} else {
				std::cout << "    ( 2.5 )   other\n" << std::flush;
				// Index of the first umpire selected
				int u1x = std::get<0>(feasible_perms[0]);

				// Index of the second umpire selected
				int u2x = std::get<1>(feasible_perms[0]);

				// The corresponding time slot index
				int sx = std::get<2>(feasible_perms[0]);

				std::tuple<int, int, int> best_exchange = feasible_perms[0];
				
				int best_cost = umps[u1x].TotalDistanceWithExchange(umps[u2x], sx);
				int curr_cost = 0;

				for (std::size_t i = 1; i < feasible_perms.size(); i++) {
					u1x = std::get<0>(feasible_perms[i]);
					u2x = std::get<1>(feasible_perms[i]);
					sx = std::get<2>(feasible_perms[i]);

					curr_cost = umps[u1x].TotalDistanceWithExchange(umps[u2x], sx);

					if (curr_cost < best_cost) {
						best_cost = curr_cost;
						best_exchange = feasible_perms[i];
					}
				}

				return best_exchange;

			}
		}
	}

	std::size_t length();
	std::string ToString();
};

struct Instance {
	
};