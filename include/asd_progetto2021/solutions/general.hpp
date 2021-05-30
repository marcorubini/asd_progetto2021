#pragma once
#include <asd_progetto2021/solutions/no_tour.hpp>
#include <asd_progetto2021/solutions/selection_only.hpp>
#include <asd_progetto2021/solutions/tsp_only.hpp>
#include <asd_progetto2021/utilities/improvements.hpp>

inline auto solve_general (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const time_start = Clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<Milli> (Clock::now () - time_start);
  };

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

  // ---

  auto route = solve_tsp_only (dataset, rng, remaining / 5);

  if (total_weight <= dataset.glove_capacity () || constant_weight) {
    auto selected = solve_selection_only_greedy_energy (dataset);
    auto matched = solve_no_tour_incremental_matching (dataset, selected);
    auto improved = improve (route, matched, rng, remaining - elapsed ());
    return {std::move (route), std::move (matched)};
  }

  if (constant_energy) {
    auto selected = solve_selection_only_greedy_weight (dataset);
    auto matched = solve_no_tour_incremental_matching (dataset, selected);
    auto improved = improve (route, matched, rng, remaining - elapsed ());
    return {std::move (route), std::move (matched)};
  }

  if (real_capacity * dataset.num_stones () <= 10000000) {
    auto selected = solve_selection_only_knapsack (dataset);

    std::sort (selected.begin (), selected.end (), [&] (int a, int b) {
      return dataset.stone (a).energy > dataset.stone (b).energy;
    });

    auto matched = solve_no_tour_incremental_matching (dataset, selected);
    auto improved = improve (route, matched, rng, remaining - elapsed ());
    return {std::move (route), std::move (matched)};
  }

  auto const selection_strategies = {solve_selection_only_greedy_energy, //
    solve_selection_only_greedy_weight,
    solve_selection_only_greedy_ratio};

  auto matched = StoneMatching (dataset);
  auto score = evaluate (route, matched);

  for (auto strategy : selection_strategies) {
    auto selected = strategy (dataset);
    auto new_matched = solve_no_tour_incremental_matching (dataset, selected);
    auto eval = evaluate (route, new_matched);
    if (eval.score > score.score) {
      score = eval;
      matched = std::move (new_matched);
    }
  }

  auto improved = improve (route, matched, rng, remaining - elapsed ());
  return {std::move (route), std::move (matched)};
}