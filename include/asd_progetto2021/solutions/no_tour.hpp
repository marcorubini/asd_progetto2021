#pragma once
#include <asd_progetto2021/dataset/stone_matching.hpp>
#include <asd_progetto2021/dataset/tour.hpp>
#include <asd_progetto2021/opt/bipartite_matching.hpp>
#include <asd_progetto2021/opt/knapsack.hpp>
#include <asd_progetto2021/utilities/timer.hpp>

#include <iostream>
#include <numeric>
#include <random>

inline auto solve_no_tour (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> StoneMatching
{
  auto const timer = Timer ();

  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto const weight_fn = [&] (int id) -> int { return dataset.stone (id).weight; };
  auto const value_fn = [&] (int id) -> long long { return dataset.stone (id).energy; };

  std::sort (indices.begin (), indices.end (), [&] (int a, int b) { return weight_fn (a) > weight_fn (b); });
  auto knapsack_result = Knapsack::knapsack (dataset.glove_capacity (), indices.begin (), indices.end (), weight_fn, value_fn);

  ASSERT (knapsack_result.exact == true);

  auto matching_result = [&] () {
    if (knapsack_result.selection.size () < dataset.num_cities ()) {
      // unweighted matching
      auto bip = BipartiteMatching::HopcroftKarp (knapsack_result.selection.size (), dataset.num_cities ());
      for (int i = 0; i < knapsack_result.selection.size (); ++i)
        for (auto j : dataset.cities_with_stone (knapsack_result.selection[i]))
          bip.add (i, j);

      auto matched = bip.solve ();
      auto result = std::vector<std::pair<int, int>> ();
      bip.for_each_edge ([&] (int from, int to) { result.emplace_back (knapsack_result.selection[from], to); });
      return result;
    } else {
      ASSERT (false);
      std::terminate ();
    }
  }();

  auto matching = StoneMatching (dataset);
  for (auto e : matching_result)
    matching.match (e.first, e.second);
  return matching;
}