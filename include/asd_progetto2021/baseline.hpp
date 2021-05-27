#pragma once
#include <asd_progetto2021/dataset.hpp>
#include <random>

// Start from the first vertex, going forwards, take a random stone until
// the glove is at full capacity.
inline auto select_random_stones (Dataset const& dataset, //
  Route const& route) -> StoneMatching
{
  auto rng = std::mt19937 (std::random_device {}());
  auto matching = StoneMatching (dataset);

  auto best_score = evaluate (route, matching);
  auto weight = 0;

  route.for_each_edge ([&] (int from, int to) {
    auto available_stones = dataset.stones_at_city (from);

    // erase matched stones
    [&] () {
      auto const is_matched = [&matching] (int stone_id) { return matching.is_stone_matched (stone_id); };

      auto last = std::remove_if (available_stones.begin (), available_stones.end (), is_matched);
      available_stones.erase (last, available_stones.end ());
    }();

    // take a prefix of at most 15 stones
    if (available_stones.size () > 15)
      available_stones.resize (15);

    // shuffle
    std::shuffle (available_stones.begin (), available_stones.end (), rng);

    // take the best stone
    int best_stone = -1;
    for (int stone_id : available_stones) {
      if (!matching.is_stone_matched (stone_id)) {
        if (weight + dataset.stone (stone_id).weight <= dataset.glove_capacity ()) {

          // try to match and compute a new score
          matching.match (stone_id, from);
          auto const new_score = evaluate (route, matching);
          if (new_score.score > best_score.score) {
            best_score = new_score;
            best_stone = stone_id;
          }

          // unmatch to try new stones
          matching.unmatch_stone (stone_id);
        }
      }
    }

    if (best_stone != -1) {
      matching.match (best_stone, from);
      weight += dataset.stone (best_stone).weight;
    }
  });

  return matching;
}

inline auto select_random_route (Dataset const& dataset) -> Route
{
  auto rng = std::mt19937 (std::random_device {}());
  auto result = Route (dataset);

  auto order = std::array<int, MAX_CITIES + 1> {};
  std::iota (order.begin (), order.begin () + dataset.num_cities (), 0);
  std::shuffle (order.begin (), order.begin () + dataset.num_cities (), rng);
  for (int i = 0; i < dataset.num_cities (); ++i)
    if (order[i] != dataset.starting_city ())
      result.push_back (order[i]);

  result.push_back (dataset.starting_city ());
  return result;
}

inline auto select_greedy_route (Dataset const& dataset) -> Route
{
  auto result = Route (dataset);

  while (result.size () != dataset.num_cities ()) {
    int curr_best = -1;
    int best_dist = 1u << 30;
    for (int i = 0; i < dataset.num_cities (); ++i) {
      if (!result.is_visited (i)) {
        if (dataset.distance (i, result.back ()) < best_dist) {
          best_dist = dataset.distance (i, result.back ());
          curr_best = i;
        }
      }
    }
    result.push_back (curr_best);
  }

  result.push_back (dataset.starting_city ());
  return result;
}

inline auto baseline (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_random_route (dataset);
  auto const stones = select_random_stones (dataset, route);
  return {std::move (route), std::move (stones)};
}

inline auto baseline_greedy (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_greedy_route (dataset);
  auto const stones = select_random_stones (dataset, route);
  return {std::move (route), std::move (stones)};
}