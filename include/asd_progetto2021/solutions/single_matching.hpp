#pragma once
#include <asd_progetto2021/dataset/stone_matching.hpp>
#include <asd_progetto2021/dataset/tour.hpp>
#include <asd_progetto2021/opt/bipartite_matching.hpp>
#include <asd_progetto2021/opt/knapsack.hpp>
#include <asd_progetto2021/utilities/timer.hpp>

#include <iostream>
#include <numeric>
#include <random>

inline auto solve_single_matching (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const timer = Timer ();

  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto const weight_fn = [&] (int id) -> int { return dataset.stone (id).weight; };
  auto const value_fn = [&] (int id) -> long long { return dataset.stone (id).energy; };

  auto selected = Knapsack::knapsack (dataset.glove_capacity (), indices.begin (), indices.end (), weight_fn, value_fn);

  ASSERT (selected.exact == true);

  auto matching = StoneMatching (dataset);
  for (auto i : selected.selection)
    if (matching.fits (dataset.stone (i).weight))
      matching.match (i, dataset.cities_with_stone (i).at (0));

  indices.resize (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  Tsp::tsp (
    indices.data (),
    indices.size (),
    dataset.starting_city (),
    [&] (int from, int to) { return dataset.distance (from, to); },
    rng,
    allowed_ms - timer.elapsed_ms ());

  auto tour = SimpleRoute (dataset, indices.data (), indices.data () + indices.size ());
  return {std::move (tour), std::move (matching)};
}