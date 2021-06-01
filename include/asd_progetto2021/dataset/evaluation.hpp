#pragma once
#include <asd_progetto2021/dataset/stone_matching.hpp>
#include <asd_progetto2021/dataset/tour.hpp>

struct Evaluation
{
  double score;
  int energy;
  double travel_time;
};

inline auto evaluate (SimpleRoute const& route, StoneMatching const& matching) -> Evaluation
{
  auto const& dataset = route.dataset ();

  int curr_weight = 0;
  double travel_time = 0;

  route.for_each_edge ([&] (int from, int to) {
    if (matching.is_city_matched (from))
      curr_weight += dataset.stone (matching.matched_stone (from)).weight;
    travel_time += dataset.travel_time (from, to, curr_weight);
  });

  return Evaluation {dataset.final_score (matching.energy (), travel_time), matching.energy (), travel_time};
}