#pragma
#include <asd_progetto2021/dataset.hpp>

template<class Heuristic>
inline auto select_stones_matching (Dataset const& dataset, Heuristic fn)
{
  auto edges = std::vector<std::tuple<double, int, int>> ();
  for (int i = 0; i < dataset.num_stones (); ++i)
    for (int j : dataset.cities_with_stone (i))
      edges.emplace_back (fn (i, j), i, j);

  std::sort (edges.begin (), edges.end ());

  auto matching = StoneMatching (dataset);
  for (auto edge : edges) {
    auto cost = std::get<0> (edge);
    auto stone_id = std::get<1> (edge);
    auto city_id = std::get<2> (edge);

    if (matching.is_stone_matched (stone_id) == false && matching.is_city_matched (city_id) == false)
      if (matching.weight () + dataset.stone (stone_id).weight <= dataset.glove_capacity ())
        matching.match (stone_id, city_id);
  }

  return matching;
}