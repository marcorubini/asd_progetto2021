#pragma once
#include <asd_progetto2021/dataset/dataset.hpp>
#include <asd_progetto2021/opt/knapsack.hpp>

#include <iostream>
#include <numeric>
#include <random>

inline auto solve_selection_only (Dataset const& dataset, std::mt19937& rng) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto result = Knapsack::knapsack (
    dataset.glove_capacity (),
    indices.begin (),
    indices.end (),
    [&] (int id) -> int { return dataset.stone (id).weight; },
    [&] (int id) -> long long { return dataset.stone (id).energy; });

  ASSERT (result.exact == true);

  return result.selection;
}