#pragma once
#include <asd_progetto2021/utilities/matching.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <chrono>
#include <random>

inline auto tsp_bootstrap_greedy (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
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

    tour.reverse (i, tour.city_index (curr_best));
  }

  return tour;
}

inline auto tsp_bootstrap_random (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
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

template<class Strategy>
inline auto tsp_bootstrap (Dataset const& dataset, Strategy strategy, std::mt19937& rng, int rounds) -> SimpleRoute
{
  auto result = SimpleRoute (dataset);
  while (rounds-- > 0) {
    auto curr = strategy (dataset, rng);
    if (curr.length () < result.length ())
      result = curr;
  }
  return result;
}

// ---

inline auto tsp_improve_all (SimpleRoute& route, std::mt19937& rng) -> int
{
  auto const& dataset = route.dataset ();
  auto indices = std::vector<int> (dataset.num_cities () - 1);
  std::iota (indices.begin (), indices.end (), 1);
  std::shuffle (indices.begin (), indices.end (), rng);

  int tot = 0;
  int threshold = 2;
  while (threshold > 0) {
    bool improved = false;
    for (int i : indices) {
      for (int j = i + 1; j < dataset.num_cities (); ++j) {
        auto change = route.reverse_profit (i, j);
        if (change <= -threshold) {
          tot += change;
          route.reverse (i, j);
          improved = true;
        }
      }
    }
    if (!improved)
      threshold /= 2;
  }
  return tot;
}

inline auto tsp_solve (Dataset const& dataset, std::mt19937& rng, std::chrono::milliseconds duration) -> SimpleRoute
{
  auto time_start = std::chrono::steady_clock::now ();

  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now () - time_start);
  };

  auto result = SimpleRoute (dataset);
  auto greedy = tsp_bootstrap_greedy (dataset, rng);
  if (result.length () == greedy.length ())
    return greedy;
  result = greedy;

  while (elapsed () < duration) {
    for (auto strategy : {tsp_bootstrap_greedy}) {
      auto curr = tsp_bootstrap (dataset, strategy, rng, 2);
      tsp_improve_all (curr, rng);
      if (curr.length () < result.length ()) {
        result = curr;
      }
    }
  }
  return result;
}
