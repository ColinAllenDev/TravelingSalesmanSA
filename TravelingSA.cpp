/* Traveling Salesman - Simulated Annealing Implementation - Colin Allen*/
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <cmath>

/* Constants */
#define TOWNS_MAX 25
#define X_MAX 25
#define Y_MAX 25


struct Town {
	int x, y;

	bool operator==(const Town& t) {
		if (x == t.x && y == t.y) return true;
		else return false;
	}
};

/* The path of our salesman */
struct Path {
	std::vector<Town> towns;	// The list of (x,y) coordinates of each town
	double h_cost;				// The total heuristic cost of all towns in the path
};

/* Random Utility Functions */
double d_rand() {
	double r = (double)rand() / (double)RAND_MAX;
	return r;
}

int i_rand(int min, int max) {
	int r = rand() % max + min;
	return r;
}

/* Get a random index to represent a segment of our path vector */
std::vector<int> RandomSegment(std::vector<Town>* p) {
	std::vector<int> tmp;
	
	// Take out a random segment of our path
	int s_index, e_index;
	bool success = false;
	while (!success) {
		s_index = i_rand(1, p->size());
		e_index = i_rand(1, p->size());
		
		if (s_index != e_index) success = true;
	}

	if (s_index > e_index) std::swap(e_index, s_index);

	tmp.push_back(s_index);
	tmp.push_back(e_index);

	return tmp;
}

/* Used to generate our random initial path of length n towns */
Path GeneratePath(int n) {
	Path p_tmp;
	Town t_tmp;

	for (int i = 0; i < n-1; ++i) {
		bool success = false;
		while (!success) {
			t_tmp.x = i_rand(0, X_MAX);
			t_tmp.y = i_rand(0, Y_MAX);

			// Check if town is already in path
			if (std::find(p_tmp.towns.begin(), p_tmp.towns.end(), t_tmp) == p_tmp.towns.end()) {
				success = true;
			}
		}

		p_tmp.towns.push_back(t_tmp);
	}

	return p_tmp;
}

std::vector<Town> SliceVector(std::vector<Town> const& v, int s, int e) {
	std::vector<Town> tmp(e - s);
	std::copy(v.begin() + s, v.begin() + e, tmp.begin());
	return tmp;
}

void PrintVector(std::vector<Town> const& v) {
	for (auto i : v) {
		std::cout << "[" << i.x << "][" << i.y << "]" << std::endl;
	}

	std::cout << std::endl;
}

/* Used to generate a new path by either transporting or reversing a segment of the original */
Path GeneratePath(Path* p, int ct) {
	// Copy our original vector
	Path p_tmp;
	std::vector<Town> t_tmp(p->towns);

	// Take out a random segment of our path
	std::vector<int> s_index = RandomSegment(&t_tmp);
	// Reversing the segment
	if (ct == 0) {
		// 'pop' our segment out of the original vector
		std::vector<Town> segment = SliceVector(t_tmp, s_index[0], s_index[1]);
		t_tmp.erase(t_tmp.begin() + s_index[0], t_tmp.begin() + s_index[1]);

		// Reverse our segment
		std::reverse(segment.begin(), segment.end());

		// Insert it back into our vector
		t_tmp.insert(t_tmp.begin() + s_index[0], segment.begin(), segment.end());
	}
	/* Transporting the segment */
	else if (ct == 1) {
		// 'Remove' our segment from the original path
		std::vector<Town> t2_tmp(t_tmp);
		t2_tmp.erase(t2_tmp.begin() + s_index[0], t2_tmp.begin() + s_index[1]);

		// Find a new index to append our segment to
		int r_index = i_rand(1, t2_tmp.size());

		// Append our segment
		std::vector<Town> segment = SliceVector(t_tmp, s_index[0], s_index[1]);
		t2_tmp.insert(t2_tmp.begin() + r_index, segment.begin(), segment.end());

		t_tmp = t2_tmp;
	}
	
	p_tmp.towns = t_tmp;
	return p_tmp;
}

/* Exponential Cooling Function */
void ExpCooling(double* T, double T_step) {
	*T = *T * exp(-T_step);
}

/* Hueristic Function - Euclidean Distance*/
double EuclideanDistance(Path* p) {
	double cost = 0;	// Total heuristic cost of this path

	// Iterate through each town in the path
	for (auto it = p->towns.begin(); it != p->towns.end() - 1; ++it) {
		Town* t1 = &(*it);
		Town* t2 = &(*std::next(it, 1));

		double x_delta = abs(t2->x - t1->x);
		double y_delta = abs(t2->y - t1->y);
		double ed = sqrt((x_delta * x_delta) + (y_delta * y_delta));

		cost += ed;
	}

	return cost;
}

/* Program Entry Point */
int main() {
	// Initialize random seed
	srand(time(NULL));

	// Create a random initial path and set it as our current path
	Path p_curr = GeneratePath(TOWNS_MAX);
	PrintVector(p_curr.towns);

	// Set our initial temperature values
	double T_max = 1000;
	double T_min = 0.09;
	double T_step = 0.01;

	// Iterate until tempurature reaches its minimum value using exponential cooling
	for (double T = T_max; T > T_min; ExpCooling(&T, T_step)) {
		bool pathFound = false;	// If a more efficient path is found
		int compareSteps = 100 * p_curr.towns.size(); // Arbitrary number of comparisons.
		while (compareSteps >= 0 && pathFound == false) {
			// Get a random segment of our path and transform it based on a 'coin toss'
			Path p_new = GeneratePath(&p_curr, i_rand(0,2));

			// Append the start vector to the end when comparing heuristics (full circuit)
			Path p_curr_circuit;
			p_curr_circuit.towns = p_curr.towns;
			p_curr_circuit.towns.push_back(p_curr.towns.at(0));
			Path p_new_circuit;
			p_new_circuit.towns = p_new.towns;
			p_new_circuit.towns.push_back(p_new.towns.at(0));

			// Calculate heuristics
			double h_current = EuclideanDistance(&p_curr_circuit);
			double h_new = EuclideanDistance(&p_new_circuit);
			double h_delta = h_new - h_current;

			if (h_delta < 0) {
				p_curr = p_new;
				pathFound = true;
			}
			else if (exp((-h_delta) / T) > d_rand()) {
				p_curr = p_new;
				pathFound = true;
			}
			else if (compareSteps == 0) {
				PrintVector(p_curr.towns);
				exit(EXIT_SUCCESS);
			}
			else {
				compareSteps--;
				continue;
			}
		}
	}
	
	PrintVector(p_curr.towns);
}