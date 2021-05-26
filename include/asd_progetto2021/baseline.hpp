#pragma once
#include <asd_progetto2021/common.hpp>
#include <random>

// Start from the first vertex, going forwards, take a random stone until
// the glove is at full capacity.
inline auto select_random_stones (Dataset const& dataset, //
  std::vector<int> const& route) -> std::vector<int>
{
  auto rng = std::mt19937 (std::random_device {}());
  auto stones = std::vector<int> (dataset.num_stones (), -1);

  auto best_score = evaluate (dataset, stones, route);
  auto weight = 0;
  for (int i = 0; i + 1 < (int)route.size (); ++i) {
    auto available = dataset.stones_stored_at (route[i]);
    std::shuffle (available.begin (), available.end (), rng);
    if (available.size () > 15)
      available.resize (15);

    for (auto const stone_id : available) {
      if (stones[stone_id] == -1 && weight + dataset.stone (stone_id).weight <= dataset.glove_capacity ()) {
        stones[stone_id] = i;
        auto new_score = evaluate (dataset, stones, route);
        if (new_score.score > best_score.score) {
          best_score = new_score;
          weight += dataset.stone (stone_id).weight;
          break;
        } else {
          stones[stone_id] = -1;
        }
      }
    }
  }
  return stones;
}

inline auto select_random_route (Dataset const& dataset) -> std::vector<int>
{
  auto rng = std::mt19937 (std::random_device {}());
  auto result = std::vector<int> (dataset.num_vertices ());
  std::iota (result.begin (), result.end (), 0);
  std::swap (result[0], result[dataset.starting_city ()]);
  std::shuffle (result.begin () + 1, result.end (), rng);
  result.push_back (dataset.starting_city ());
  return result;
}

inline auto select_greedy_route (Dataset const& dataset) -> std::vector<int>
{
  auto result = std::vector<int> ();
  result.reserve (dataset.num_vertices () + 1);

  auto visited = std::vector<bool> (dataset.num_vertices ());

  visited[dataset.starting_city ()] = true;
  result.push_back (dataset.starting_city ());

  while (result.size () != dataset.num_vertices ()) {
    int curr_best = -1;
    int best_dist = 1u << 30;
    for (int i = 0; i < dataset.num_vertices (); ++i) {
      if (!visited[i] && dataset.distance (i, result.back ()) < best_dist) {
        best_dist = dataset.distance (i, result.back ());
        curr_best = i;
      }
    }

    visited[curr_best] = true;
    result.push_back (curr_best);
  }
  result.push_back (dataset.starting_city ());
  return result;
}

inline auto baseline (Dataset const& dataset) -> std::pair<std::vector<int>, std::vector<int>>
{
  auto const route = select_random_route (dataset);
  auto const stones = select_random_stones (dataset, route);
  return {std::move (stones), std::move (route)};
}

inline auto baseline_greedy (Dataset const& dataset) -> std::pair<std::vector<int>, std::vector<int>>
{
  auto const route = select_greedy_route (dataset);
  auto const stones = select_random_stones (dataset, route);
  return {std::move (stones), std::move (route)};
}