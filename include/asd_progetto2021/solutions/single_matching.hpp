#pragma once
#include <asd_progetto2021/solutions/selection_only.hpp>
#include <asd_progetto2021/solutions/tsp_only.hpp>
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/improvements.hpp>
#include <asd_progetto2021/utilities/matching.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>

inline auto solve_single_matching (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const time_start = Clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<Milli> (Clock::now () - time_start);
  };

  auto const total_weight = [&] () {
    int tot = 0;
    for (auto s : dataset.stones ())
      tot += s.weight;
    return tot;
  }();

  auto const constant_weight = [&] () {
    for (auto s : dataset.stones ())
      if (s.weight != dataset.stone (0).weight)
        return false;
    return true;
  }();

  auto const constant_energy = [&] () {
    for (auto s : dataset.stones ())
      if (s.energy != dataset.stone (0).energy)
        return false;
    return true;
  }();

  auto const real_capacity = std::min (total_weight, dataset.glove_capacity ());

  // ----

  if (total_weight <= dataset.glove_capacity ()) {
    // take everything
    auto matching = StoneMatching (dataset);
    for (int i = 0; i < dataset.num_stones (); ++i)
      matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }

  if (constant_weight) {
    // maximize energy
    auto matching = StoneMatching (dataset);
    for (auto i : solve_selection_only_greedy_energy (dataset))
      if (matching.fits (dataset.stone (i).weight))
        matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }

  if (constant_energy) {
    // minimize weight
    auto matching = StoneMatching (dataset);
    for (auto i : solve_selection_only_greedy_weight (dataset))
      if (matching.fits (dataset.stone (i).weight))
        matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }

  if (real_capacity * dataset.num_stones () <= 10000000) {
    // knapsack
    auto matching = StoneMatching (dataset);
    for (auto i : solve_selection_only_knapsack (dataset))
      if (matching.fits (dataset.stone (i).weight))
        matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }

  std::terminate ();
}