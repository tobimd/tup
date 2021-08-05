#include "../../include/structs.h"

Umpire::Umpire(const int id, Array2D dist, const int n_teams) : n_teams_(n_teams), id(id), dist(dist) { 
	n_size_ = 2 * n_teams - 2;
	p_path = new Match[n_size_];
}

void Umpire::Delete() {
	dist.Delete();
	delete[] p_path;
}

void Umpire::AddToPath(Match match) {
	p_path[n_length_] = match;

	++n_length_;
}

void Umpire::Backtrack() {
	--n_length_;
}

int Umpire::DistanceTo(const int home_venue) {
	return dist[p_path[n_length_ - 1].home - 1][home_venue - 1];
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

	for (int i = start; i > start - q1 && i >= 0; i--)
		if (p_path[i].home == home_venue) count++;

	return count;
}

int Umpire::TeamVisitViolations(const int q2) {
	int count = 0;
	int team_visits[n_teams_];
	int start = n_length_ - 1;

	for (int i = 0; i < n_teams_; i++)
		team_visits[i] = 0;

	for (int i = start; i > start - q2 && i >= 0; i--){ 
		team_visits[p_path[i].home - 1]++;
		team_visits[p_path[i].visit - 1]++;
	}

	for (int i = 0; i < n_teams_; i++)
		if (team_visits[i] > 1) count++;

	return count;
}

std::string Umpire::ToString() {
	std::string output = "<Umpire: id=" + std::to_string(id) + ", path[" + std::to_string(n_length_) + "]=[";

	if (n_length_ > 0) {
		output += p_path[0].ToString();
		
		for (int i = 1; i < n_length_; i++)
			output += ", " + p_path[i].ToString();
	}

	output += "] >\n";
	return output;
}

void Umpire::ExchangeMatch(const Umpire &other, const std::size_t time_slot) {
	Match tmp = other.p_path[time_slot];

	other.p_path[time_slot] = p_path[time_slot];
	p_path[time_slot] = tmp;
}
