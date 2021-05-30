#pragma once
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/matching.hpp>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>

inline auto solve_selection_only_greedy_energy (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto compare = [&] (int a, int b) {
    auto s1 = dataset.stone (a);
    auto s2 = dataset.stone (b);
    return s1.energy > s2.energy;
  };

  std::sort (indices.begin (), indices.end (), compare);
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int x : indices) {
    auto s = dataset.stone (x);
    if (weight + s.weight <= dataset.glove_capacity ()) {
      weight += s.weight;
      result.push_back (x);
    }
  }

  return result;
}

inline auto solve_selection_only_greedy_weight (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto compare = [&] (int a, int b) {
    auto s1 = dataset.stone (a);
    auto s2 = dataset.stone (b);
    return s1.weight < s2.weight;
  };

  std::sort (indices.begin (), indices.end (), compare);
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int x : indices) {
    auto s = dataset.stone (x);
    if (weight + s.weight <= dataset.glove_capacity ()) {
      weight += s.weight;
      result.push_back (x);
    }
  }

  return result;
}

inline auto solve_selection_only_knapsack (Dataset const& dataset) -> std::vector<int>
{
  auto const n = dataset.num_stones ();
  auto const c = [&] () {
    int tot = 0;
    for (auto x : dataset.stones ())
      tot = std::min (tot + x.weight, dataset.glove_capacity ());
    return tot;
  }();

  auto dp = std::vector<int> (c + 1);
  auto taken = std::vector<std::vector<bool>> (n, std::vector<bool> (c + 1));

  for (int i = 0; i < n; ++i) {
    auto stone = dataset.stone (i);
    for (int j = c; j >= stone.weight; --j) {
      if (dp[j - stone.weight] + stone.energy > dp[j]) {
        dp[j] = dp[j - stone.weight] + stone.energy;
        taken[i][j] = true;
      }
    }
  }
  auto result = std::vector<int> ();
  int capacity = c;
  int energy = 0;
  for (int i = n - 1; i >= 0; --i) {
    if (taken[i][capacity]) {
      capacity -= dataset.stone (i).weight;
      energy += dataset.stone (i).energy;
      result.push_back (i);
    }
  }

  return result;
}

inline auto solve_selection_only_greedy_ratio (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto compare = [&] (int a, int b) {
    auto s1 = dataset.stone (a);
    auto s2 = dataset.stone (b);
    auto r1 = (double)s1.energy / (s1.weight + 1);
    auto r2 = (double)s2.energy / (s2.weight + 1);
    return r1 > r2;
  };

  std::sort (indices.begin (), indices.end (), compare);
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int x : indices) {
    auto s = dataset.stone (x);
    if (weight + s.weight <= dataset.glove_capacity ()) {
      weight += s.weight;
      result.push_back (x);
    }
  }

  return result;
}

inline auto solve_selection_only_small_weight (Dataset const& dataset) -> std::vector<int>
{
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int i = 0; i < dataset.num_cities (); ++i)
    if (weight + dataset.stone (i).weight <= dataset.glove_capacity ()) {
      weight += dataset.stone (i).weight;
      result.push_back (i);
    }
  return result;
}

inline auto solve_selection_only (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> std::vector<int>
{
  auto const total_weight = [&] () {
    long long tot = 0;
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

  auto const real_capacity = std::min (total_weight, (long long)dataset.glove_capacity ());

  if (total_weight <= dataset.glove_capacity ()) {
    return solve_selection_only_small_weight (dataset);
  }

  if (constant_energy) {
    return solve_selection_only_greedy_weight (dataset);
  }

  if (constant_weight) {
    return solve_selection_only_greedy_energy (dataset);
  }

  if (real_capacity * dataset.num_stones () <= 10000000) {
    return solve_selection_only_knapsack (dataset);
  }

  auto const sum_energy = [&dataset] (std::vector<int> const& selection) {
    auto energy = 0ll;
    for (auto i : selection)
      energy += dataset.stone (i).energy;
    return energy;
  };

  auto const strategies = {//
    solve_selection_only_greedy_weight,
    solve_selection_only_greedy_energy,
    solve_selection_only_greedy_ratio};

  auto result = solve_selection_only_small_weight (dataset);
  auto result_energy = sum_energy (result);
  for (auto s : strategies) {
    auto curr = s (dataset);
    auto curr_energy = sum_energy (curr);
    if (curr_energy > result_energy) {
      result = curr;
      result_energy = curr_energy;
    }
  }

  return result;
}