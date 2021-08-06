#include "../../include/structs.h"

int clamp(int value, const int min, const int max) {
	value = value < min ? min : value;
	return value > max ? max : value;
}

Umpire::Umpire(const int id, Array2D dist, const int n_teams) : n_teams_(n_teams), id(id), dist(dist) { 
	n_size_ = 2 * n_teams - 2;
	p_path = new Match[n_size_];
}

Umpire& Umpire::operator=(Umpire&& other) {
	id = std::move(other.id);
	n_length_ = std::move(other.n_length_);
	n_size_ = std::move(other.n_size_);
	n_teams_ = std::move(other.n_teams_);

	p_path = std::move(other.p_path);
	dist = std::move(other.dist);
	
	return *this;
}

void Umpire::Delete() {
	dist.Delete();
	delete[] p_path;
}

void Umpire::SetMatches(const std::vector<Match> *matches) {
	for (int i = 0; i < n_length_; i++) {
		p_path[i] = (*matches)[i];
	}
}

void Umpire::AddToPath(Match match) {
	p_path[n_length_] = match;

	++n_length_;
}

void Umpire::Backtrack() {
	--n_length_;
}

std::vector<Match> Umpire::GetPath() {
	std::vector<Match> path;
	path.reserve(n_length_);

	for (int i = 0; i < n_length_; i++)
		path[i] = p_path[i];

	return path;
}

std::vector<Match> Umpire::GetPathWithExchange(Umpire &other, const int time_slot) {
	std::vector<Match> path;
	path.reserve(n_length_);

	for (int i = 0; i < n_length_; i++) {
		if (time_slot == i) {
			path[i] = other.p_path[i];
		} else {
			path[i] = p_path[i];
		}
	}

	return path;
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

int Umpire::TotalDistance() {
	int cost = 0;

	for (int i = 1; i < n_length_; i++)
		cost += dist[p_path[i - 1].home - 1][p_path[i].home - 1];

	return cost;
}

int Umpire::TotalDistanceWithExchange(Umpire &other, const int time_slot) {
	int cost = 0;

	for (int i = 1; i < n_length_; i++) {
		if (i - 1 == time_slot)
			cost += dist[other.p_path[i - 1].home - 1][p_path[i].home -1];

		else if (i == time_slot)
			cost += dist[p_path[i - 1].home - 1][other.p_path[i].home -1];

		else
			cost += dist[p_path[i - 1].home - 1][p_path[i].home - 1];
	}

	return cost;
}

void Umpire::SwapMatch(Umpire &other, const std::size_t time_slot) {
	Match tmp = other.p_path[time_slot];

	other.p_path[time_slot] = p_path[time_slot];
	p_path[time_slot] = tmp;
}

std::size_t Umpire::length() {
	return (unsigned long) n_length_;
}

std::string Umpire::ToString() {
	std::string output = "[+] ump " + std::to_string(id) + ": ";

	if (n_length_ > 0) {
		output += std::to_string(p_path[0].home);
		
		for (int i = 1; i < n_length_; i++)
			output += "->" + std::to_string(p_path[i].home);
	}
	return output;
}
