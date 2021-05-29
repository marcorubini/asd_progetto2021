#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

int main (int argc, char** argv)
{
  auto input = std::ifstream (argv[1]);
  auto solution = std::ifstream (argv[2]);

  if (!input)
    std::terminate ();

  if (!solution)
    std::terminate ();

  int num_cities;
  int starting_city;

  input >> num_cities >> starting_city;

  int num_stones;
  int glove_capacity;
  double glove_resistance;
  double min_velocity;
  double max_velocity;
  input >> num_stones >> glove_capacity >> glove_resistance >> min_velocity >> max_velocity;

  auto edges = std::vector<std::pair<int, int>> ();
  auto stones = std::vector<std::pair<int, int>> (num_stones);

  for (int i = 0; i < num_stones; ++i)
    input >> stones[i].first >> stones[i].second;

  for (int i = 0; i < num_stones; ++i) {
    int len;
    input >> len;

    for (int j = 0; j < len; ++j) {
      int city;
      input >> city;
      edges.emplace_back (i, city);
    }
  }

  auto valid_matches = std::vector<std::unordered_set<int>> (num_stones);
  for (auto e : edges)
    valid_matches[e.first].insert (e.second);

  auto cost_matrix = std::vector<std::vector<int>> (num_cities, std::vector<int> (num_cities, 0));

  for (int i = 0; i < num_cities; ++i) {
    for (int j = 0; j < i; ++j) {
      input >> cost_matrix[i][j];
      cost_matrix[j][i] = cost_matrix[i][j];
    }
  }

  // read solution

  double score;
  int glove_energy;
  double travel_time;
  solution >> score >> glove_energy >> travel_time;

  auto stone_matching = std::vector<int> (num_stones);

  for (int i = 0; i < num_stones; ++i)
    solution >> stone_matching[i];

  auto matched_cities = std::vector<int> (num_cities, -1);
  for (int i = 0; i < num_stones; ++i) {
    if (stone_matching[i] < -1 || stone_matching[i] >= num_cities) {
      std::cerr << "Stone " << i << " has wrong city\n";
      std::terminate ();
    }

    if (stone_matching[i] != -1 && matched_cities[stone_matching[i]] != -1) {
      std::cerr << "Stone " << i << " matched to duplicate city\n";
      std::terminate ();
    }

    if (stone_matching[i] != -1) {
      matched_cities[stone_matching[i]] = i;
      if (valid_matches[i].count (stone_matching[i]) == 0) {
        std::cerr << "Stone " << i << " matched to invalid city\n";
        std::terminate ();
      }
    }
  }

  // compute energy and weight

  int energy = 0;
  int weight = 0;

  for (int i = 0; i < num_stones; ++i) {
    if (stone_matching[i] != -1) {
      weight += stones[i].first;
      energy += stones[i].second;
    }
  }

  if (weight > glove_capacity) {
    std::cerr << "too much weight\n";
    std::terminate ();
  }

  if (glove_energy != energy) {
    std::cerr << "inconsistent energy\n";
    std::terminate ();
  }

  // read path

  auto tour = std::vector<int> (num_cities + 1);
  for (int i = 0; i <= num_cities; ++i)
    solution >> tour[i];

  if (tour[0] != tour[num_cities]) {
    std::cerr << "tour not a loop\n";
    std::terminate ();
  }

  if (tour[0] != starting_city) {
    std::cerr << "invalid start\n";
    std::terminate ();
  }

  auto visited_cities = std::vector<bool> (num_cities, false);
  visited_cities[tour[0]] = true;
  for (int i = 1; i < num_cities; ++i) {
    if (visited_cities[tour[i]]) {
      std::cerr << "invalid tour multiple visit\n";
      std::terminate ();
    }

    visited_cities[tour[i]] = true;
  }

  // evaluate

  auto const compute_travel_time = [&] (int length, int weight) -> double {
    auto delta_vel = max_velocity - min_velocity;
    auto slow_fact = delta_vel / glove_capacity;
    auto current_velocity = std::max (min_velocity, max_velocity - weight * slow_fact);
    return length / current_velocity;
  };

  double time = 0;
  int curr_weight = 0;

  for (int i = 0; i < num_cities; ++i) {
    int c1 = tour[i];
    int c2 = tour[(i + 1) % num_cities];

    if (matched_cities[c1] != -1)
      curr_weight += stones[matched_cities[c1]].first;

    time += compute_travel_time (cost_matrix[c1][c2], curr_weight);
  }

  if (std::abs (time - travel_time) > 0.0001) {
    std::cerr << "inconsistent time\n";
    std::terminate ();
  }

  auto computed_score = energy - time * glove_resistance;

  if (std::abs (computed_score - score) > 0.0001) {
    std::cerr << "inconsistent score\n";
    std::terminate ();
  }
}