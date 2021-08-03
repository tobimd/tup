#include "../include/structs.h"

Array2D::Array2D(const int __rows, const int __cols) : rows(__rows), cols(__cols) {
	array = new int*[rows];

	for (std::size_t i = 0; i < rows; i++) {
		array[i] = new int[cols];
	}
}

// Copy constructor
Array2D::Array2D(const Array2D& other) : rows(other.rows), cols(other.cols) {
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

Array2D &Array2D::operator=(const Array2D &other) {
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
void Array2D::Delete() {
	for (std::size_t i = 0; i < rows; i++) {
		delete[] array[i];
	}

	delete[] array;
}

std::string Array2D::ToString() {
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


int* Array2D::operator[](const int index) {
	return array[index];
}

int* Array2D::operator[](const size_t index) {
	return array[index];
}

// Generates a configuration for the complete problem
// instance.
Configuration::Configuration(const int q1, const int q2, const int max_iter) {
	Configuration::q1 = q1;
	Configuration::q2 = q2;
	Configuration::max_iter = max_iter;
}

Configuration::~Configuration() {
	dist.Delete();
	oppn.Delete();
}

void Configuration::CreateArrays(const int __teams) {
	teams = __teams;

	dist = Array2D(teams, teams);
	oppn = Array2D(2*teams - 2, teams);
}

Match::Match(int home, int visit) : home(home), visit(visit) {}

std::string Match::ToString() {
	return "<Match: home=" + std::to_string(home) + ", visit=" + std::to_string(visit) + ">";
}

Umpire::Umpire(const int id, Array2D dist, const int n_teams) : n_size_(n_teams), id(id), dist(dist) { 
	p_path = new Match[n_size_];
}

void Umpire::Delete() {
	delete[] p_path;
}

void Umpire::AddToPath(const Match match) {
	p_path[n_length_] = match;

	++n_length_;
	locked = -1;
}

void Umpire::Backtrack() {
	--n_length_;
	locked = p_path[n_length_].home;
}

int Umpire::DistanceTo(const int home_venue) {
	return dist[p_path[n_length_ - 1].home][home_venue];
}

int Umpire::CountVisitsOf(const int home_venue) {
	int count = 0;

	for (int i = 0; i < n_length_; i++) {
		if (p_path[i].home == home_venue)
			count++;
	}

	return count;
}

int Umpire::HomeVisitViolations(const int home_venue, const int q1) {
	int count = 0;
	int start = n_length_ - 1;

	for (int i = start; i > start - q1; i--)
		if (p_path[i].home == home_venue) count++;

	return count;
}

int Umpire::TeamVisitViolations(const int q2) {
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

bool Umpire::HasVisited(const int home_venue) {
	for (int i = 0; i < n_length_; i++) {
		if (p_path[i].home == home_venue) {
			return true;
		}
	}

	return false;
}