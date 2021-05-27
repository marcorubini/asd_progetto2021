#pragma once
#include <asd_progetto2021/baseline.hpp>
#include <asd_progetto2021/matching.hpp>
#include <asd_progetto2021/tsp.hpp>

inline auto tsp_matching_opt (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_tour_tsp (dataset);
  auto const matching = select_stones_matching (dataset, route, [&] (int stone_id, int city_id) -> double {
    auto const idx = route.index (city_id);
    auto x = dataset.stone (stone_id).weight * (dataset.num_cities () - idx) / 2;
    auto y = dataset.stone (stone_id).energy * 100;
    return x - y;
  });
  return {route, matching};
}
