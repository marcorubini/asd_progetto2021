#pragma once
#include <asd_progetto2021/dataset/tour.hpp>
#include <asd_progetto2021/opt/tsp.hpp>

#include <algorithm>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>

inline auto solve_tsp_only (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> SimpleRoute
{
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  auto cost = Tsp::tsp (
    indices.data (),
    indices.size (),
    dataset.starting_city (), //
    [&] (int from, int to) { return dataset.distance (from, to); },
    rng,
    allowed_ms);

  return SimpleRoute (dataset, indices.data (), indices.data () + indices.size ());
}