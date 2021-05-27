#pragma once
#include <asd_progetto2021/baseline.hpp>
#include <asd_progetto2021/tsp.hpp>

inline auto tsp_matching_opt (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_tour_tsp (dataset);
  auto const matching = select_random_stones (dataset, route);
  return {route, matching};
}
