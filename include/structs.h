#pragma once

#include <iostream>    //
#include <string>      //
#include <fstream>     // -- fstream
#include <cstdlib>     // -- atoi
#include <regex>       // -- regex_replace
#include <ctime>       // 
#include <cmath>
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
	static std::string ToString(int *p_array, const std::size_t rows, const std::size_t cols);

	int* operator[](const int index);
	int* operator[](const size_t index);
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

	static void GetMatches(std::vector<Match> &matches, Configuration *config, const int time_slot);

	std::string ToString();
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