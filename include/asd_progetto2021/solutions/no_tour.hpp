#pragma once
#include <asd_progetto2021/solutions/selection_only.hpp>
#include <asd_progetto2021/utilities/bipartite_matching.hpp>
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/matching.hpp>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>

inline auto solve_no_tour_incremental_matching (Dataset const& dataset, //
  std::vector<int> const& selected) -> StoneMatching
{
  auto bip = BipartiteMatching (dataset.num_stones (), dataset.num_cities ());
  auto const add_stone = [&] (int id) {
    for (int j : dataset.cities_with_stone (id))
      bip.add (id, j);
  };

  int last = 0;
  while (last < (int)selected.size () && last < dataset.num_cities ())
    add_stone (selected[last++]);

  auto const augment = [&] (int amount) {
    while (amount-- > 0 && last < (int)selected.size ())
      add_stone (selected[last++]);
    return bip.matching ();
  };

  auto flow = bip.matching ();
  auto edges = bip.previous_edges ();
  auto next_flow = augment (5);
  while (next_flow > 0) {
    flow += next_flow;
    edges = bip.previous_edges ();
    next_flow = augment (5);
  }

  auto result = StoneMatching (dataset);
  for (auto e : edges)
    result.match (e.first, e.second);
  return result;
}

inline auto solve_no_tour (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> StoneMatching
{
  auto const time_start = Clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<Milli> (Clock::now () - time_start);
  };

  if (dataset.num_stones () == 0) {
    return StoneMatching (dataset);
  }

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

  if (constant_weight || total_weight <= dataset.glove_capacity ()) {
    // find a selection that maximizes energy, then check for a saturating matching.
    auto selected = solve_selection_only_greedy_energy (dataset);
    return solve_no_tour_incremental_matching (dataset, selected);
  }

  if (constant_energy) {
    // find a selection that minimizes weight, then check for a saturating matching.
    auto selected = solve_selection_only_greedy_weight (dataset);
    return solve_no_tour_incremental_matching (dataset, selected);
  }

  if (real_capacity * dataset.num_stones () <= 10000000) {
    // find a selection that maximizes energy, then check for a saturating matching.
    auto selected = solve_selection_only_knapsack (dataset);
    std::sort (selected.begin (), selected.end (), [&] (int a, int b) {
      return dataset.stone (a).energy > dataset.stone (b).energy;
    });
    return solve_no_tour_incremental_matching (dataset, selected);
  }

  std::terminate ();
}