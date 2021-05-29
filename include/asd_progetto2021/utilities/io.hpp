#pragma once
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/evaluation.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

template<class Fn>
using optional_result_t = typename decltype (std::declval<Fn&> () ())::first_type;

template<class Fn>
inline auto reservoir_sampling (int k, Fn fn) -> std::vector<optional_result_t<Fn>>
{
  ASSERT (k > 0);

  auto result = std::vector<optional_result_t<Fn>> ();
  result.reserve (k);

  auto rng = std::mt19937 (std::random_device {}());
  auto curr = fn ();
  int index = 0;
  while (curr.first) {
    if ((int)result.size () < k) {
      result.push_back (std::move (curr.second));
    } else {
      auto replaced = rng () % index;
      if (replaced < k)
        result[replaced] = std::move (curr.second);
    }
    curr = fn ();
    ++index;
  }

  return result;
}

inline auto read_dataset (std::istream& is) -> Dataset
{
  int num_cities = -1;
  int starting_city = -1;
  CHECK (is >> num_cities >> starting_city);

  int num_stones = -1;
  int glove_capacity = -1;
  double glove_resistance = -1;
  double min_velocity = -1;
  double max_velocity = -1;
  CHECK (is >> num_stones >> glove_capacity >> glove_resistance >> min_velocity >> max_velocity);

  auto sample_size = std::max (500000, num_cities * 120);
  auto rng = std::mt19937 (std::random_device {}());
  auto edges = std::vector<std::pair<int, int>> ();
  edges.reserve (sample_size);

  auto const reservoir_add = [&rng, &edges] (int sample_size, int index, int from, int to) {
    if (index < sample_size) {
      edges.emplace_back (from, to);
    } else {
      auto replaced = rng () % index;
      if (replaced < sample_size)
        edges[replaced] = {from, to};
    }
  };

  auto stones = StoneIndex (num_stones, num_cities);

  for (int stone_id = 0; stone_id < num_stones; ++stone_id)
    CHECK (is >> stones.stone (stone_id).weight >> stones.stone (stone_id).energy);

  int edge_index = 0;
  for (int stone_id = 0; stone_id < num_stones; ++stone_id) {
    int len = -1;
    CHECK (is >> len);

    for (int i = 0; i < len; ++i) {
      int city_id = -1;
      CHECK (is >> city_id);

      if (stones.stone (stone_id).weight <= glove_capacity) {
        reservoir_add (sample_size, edge_index++, stone_id, city_id);
      }
    }
  }

  for (auto e : edges)
    stones.store (e.first, e.second);
  stones.sort ();

  auto graph = CompleteSymmetricGraph (num_cities);
  for (int city_id = 1; city_id < num_cities; ++city_id)
    for (int other = 0; other < city_id; ++other)
      CHECK (is >> graph.distance (city_id, other));

  return Dataset (std::move (graph), //
    std::move (stones),
    Glove (glove_capacity, glove_resistance),
    starting_city,
    min_velocity,
    max_velocity);
}

inline auto write_output (std::ostream& os, SimpleRoute const& route, StoneMatching const& matching) -> void
{
  auto const eval = evaluate (route, matching);

  os << std::fixed << std::setprecision (10) << eval.score << ' ';
  os << std::fixed << std::setprecision (10) << eval.energy << ' ';
  os << std::fixed << std::setprecision (10) << eval.travel_time << '\n';

  for (int stone_id = 0; stone_id < route.dataset ().num_stones (); ++stone_id) {
    if (matching.is_stone_matched (stone_id)) {
      os << matching.matched_city (stone_id) << ' ';
    } else {
      os << -1 << ' ';
    }
  }
  os << '\n';

  route.for_each_edge ([&] (int from, int to) { os << from << ' '; });
  os << route.dataset ().starting_city () << "\n***\n";
}
