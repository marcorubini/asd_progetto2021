#pragma once

#include <asd_progetto2021/utilities/matching.hpp>
#include <asd_progetto2021/utilities/route.hpp>

inline auto solve_baseline (Dataset const& dataset) -> std::pair<SimpleRoute, StoneMatching>
{
  // Compute a greedy tour
  auto tour = [&] () {
    auto tour = SimpleRoute (dataset);

    for (int i = 1; i < dataset.num_cities (); ++i) {
      int curr_best = -1;
      int curr_dist = 1u << 30;

      for (int j = 0; j < dataset.num_cities (); ++j) {
        if (tour.city_index (j) >= i && dataset.distance (j, tour.at (i - 1)) < curr_dist) {
          curr_dist = dataset.distance (j, tour.at (i - 1));
          curr_best = j;
        }
      }
      tour.swap (i, tour.city_index (curr_best));
    }

    return tour;
  }();

  // Compute a greedy matching
  auto matching = [&] () {
    auto matching = StoneMatching (dataset);

    auto edges = dataset.stone_edges ();
    std::sort (edges.begin (), //
      edges.end (),            //
      [&] (std::pair<int, int> x, std::pair<int, int> y) {
        return dataset.stone (x.first).energy > dataset.stone (y.first).energy;
      });

    for (auto edge : edges) {
      auto stone_id = edge.first;
      auto city_id = edge.second;

      if (!matching.is_stone_matched (stone_id) && !matching.is_city_matched (city_id)) {
        auto weight = dataset.stone (stone_id).weight;
        auto energy = dataset.stone (stone_id).energy;

        if (matching.weight () + weight <= dataset.glove_capacity ()) {
          matching.match (stone_id, city_id);
        }
      }
    }

    return matching;
  }();

  return {std::move (tour), std::move (matching)};
}