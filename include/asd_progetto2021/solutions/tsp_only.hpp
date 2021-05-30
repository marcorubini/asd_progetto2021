#pragma once
#include <algorithm>
#include <asd_progetto2021/utilities/bipartite_matching.hpp>
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>

inline auto solve_tsp_only_bootstrap_greedy (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
{
  auto tour = SimpleRoute (dataset);
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  std::shuffle (indices.begin (), indices.end (), rng);

  for (int i = 1; i < dataset.num_cities (); ++i) {
    int curr_best = -1;
    int curr_dist = 1u << 30;
    for (int j : indices) {
      if (tour.city_index (j) < i)
        continue;

      if (dataset.distance (j, tour.at (i - 1)) >= curr_dist)
        continue;

      curr_dist = dataset.distance (j, tour.at (i - 1));
      curr_best = j;
    }

    tour.swap (i, tour.city_index (curr_best));
  }

  return tour;
}

inline auto solve_tsp_only_bootstrap_random (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
{
  auto tour = SimpleRoute (dataset);
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  std::shuffle (indices.begin (), indices.end (), rng);

  int size = 1;
  for (int i : indices)
    if (i != dataset.starting_city () && tour.city_index (i) >= size)
      tour.swap (size++, tour.city_index (i));
  return tour;
}

inline auto solve_tsp_only_improve_random (SimpleRoute& route, std::mt19937& rng, int rounds, int upper) -> void
{
  auto const& dataset = route.dataset ();
  auto const n = dataset.num_cities ();
  while (rounds-- > 0) {
    int x = rng () % (n - 1) + 1;
    int y = rng () % (n - 1) + 1;
    if (x > y)
      std::swap (x, y);
    if (route.reverse_profit (x, y) <= -upper)
      route.reverse (x, y);
  }
}

inline auto solve_tsp_only_improve_random_scaled (SimpleRoute& route, std::mt19937& rng, int rounds, int upper) -> void
{
  for (int k = upper; k >= 1; k /= 2)
    solve_tsp_only_improve_random (route, rng, rounds, k);
}

inline auto solve_tsp_only_improve_all (SimpleRoute& route, std::mt19937& rng, int upper) -> void
{
  auto const& dataset = route.dataset ();
  auto const n = dataset.num_cities ();
  auto indices = std::vector<int> (n - 1);
  std::iota (indices.begin (), indices.end (), 1);
  std::shuffle (indices.begin (), indices.end (), rng);
  for (auto i : indices) {
    for (int j = i + 1; j < n; ++j) {
      if (route.reverse_profit (i, j) <= -upper)
        route.reverse (i, j);
    }
  }
}

inline auto solve_tsp_only_improve_all_scaled (SimpleRoute& route, std::mt19937& rng, int upper) -> void
{
  while (upper >= 1) {
    solve_tsp_only_improve_all (route, rng, upper);
    upper /= 2;
  }
}

inline auto solve_tsp_only (Dataset const& dataset, std::mt19937& rng, Milli remaining) -> SimpleRoute
{
  auto time_start = Clock::now ();
  auto const elapsed = [time_start] () //
  { return std::chrono::duration_cast<Milli> (Clock::now () - time_start); };

  auto result = solve_tsp_only_bootstrap_greedy (dataset, rng);
  solve_tsp_only_improve_all_scaled (result, rng, 4);
  solve_tsp_only_improve_all_scaled (result, rng, 2);
  solve_tsp_only_improve_random_scaled (result, rng, 10000, 2);

  while (elapsed () < remaining * 0.8) {
    auto temp = solve_tsp_only_bootstrap_random (dataset, rng);
    solve_tsp_only_improve_all_scaled (temp, rng, 4);
    solve_tsp_only_improve_all_scaled (temp, rng, 2);
    solve_tsp_only_improve_random_scaled (result, rng, 10000, 2);
    if (temp.length () < result.length ()) {
      result = temp;
    }
  }

  while (elapsed () < remaining)
    solve_tsp_only_improve_random (result, rng, 10000, 1);
  return result;
}