#pragma once
#include <algorithm>
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/matching.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <random>

// Picks random (stone, city) pairs, then permutes the city order.
inline auto solve_random (Dataset const& dataset) -> std::pair<SimpleRoute, StoneMatching>
{
  auto route = SimpleRoute (dataset);
  auto matching = StoneMatching (dataset);

  auto rng = std::mt19937 (std::random_device {}());
  auto edges = dataset.stone_edges ();
  std::shuffle (edges.begin (), edges.end (), rng);
  for (auto e : edges) {
    auto stone_id = e.first;
    auto city_id = e.second;
    auto weight = dataset.stone (stone_id).weight;
    if (!matching.is_stone_matched (stone_id) && !matching.is_city_matched (city_id)) {
      if (weight + matching.weight () <= dataset.glove_capacity ()) {
        matching.match (stone_id, city_id);
      }
    }
  }

  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  std::shuffle (indices.begin (), indices.end (), rng);

  int curr = 1;
  for (int i : indices)
    if (i != route.at (0))
      route.swap (curr++, route.city_index (i));

  return {std::move (route), std::move (matching)};
}