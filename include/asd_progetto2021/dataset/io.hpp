#pragma once
#include <asd_progetto2021/dataset/evaluation.hpp>
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

inline auto fast_uint (FILE* is) -> int
{
  int result = 0;
  char c = getc_unlocked (is);
  while (c < '0' || c > '9')
    c = getc_unlocked (is);
  while (c >= '0' && c <= '9')
    result = result * 10 + c - '0', c = getc_unlocked (is);
  return result;
}

inline auto fast_double (FILE* is) -> double
{
  double result = 0.0;
  CHECK (fscanf (is, "%lf", &result));
  return result;
}

inline auto read_dataset (FILE* is) -> Dataset
{
  int num_cities = fast_uint (is);
  int starting_city = fast_uint (is);

  int num_stones = fast_uint (is);
  int glove_capacity = fast_uint (is);
  double glove_resistance = fast_double (is);
  double min_velocity = fast_double (is);
  double max_velocity = fast_double (is);

  auto sample_size = 6000000;
  auto rng = std::mt19937 (std::random_device {}());
  auto edges = std::vector<std::pair<std::int16_t, std::int16_t>> ();

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
  for (auto& stone : stones) {
    stone.weight = fast_uint (is);
    stone.energy = fast_uint (is);
  }

  int edge_index = 0;
  for (int stone_id = 0; stone_id < num_stones; ++stone_id) {
    int len = fast_uint (is);

    for (int i = 0; i < len; ++i) {
      int city_id = fast_uint (is);

      if (stones.stone (stone_id).weight > glove_capacity)
        continue;
      reservoir_add (sample_size, edge_index++, stone_id, city_id);
    }
  }

  for (auto e : edges)
    stones.store (e.first, e.second);
  edges = {};

  auto graph = CompleteSymmetricGraph (num_cities);
  for (int city_id = 1; city_id < num_cities; ++city_id)
    for (int other = 0; other < city_id; ++other)
      graph.distance (city_id, other) = fast_uint (is);

  return Dataset (std::move (graph), //
    std::move (stones),
    Glove (glove_capacity, glove_resistance),
    starting_city,
    min_velocity,
    max_velocity);
}

inline auto write_output (FILE* os, SimpleRoute const& route, StoneMatching const& matching) -> void
{
  auto const eval = evaluate (route, matching);

  fprintf (os, "%lf %d %lf\n", eval.score, eval.energy, eval.travel_time);

  for (int stone_id = 0; stone_id < route.dataset ().num_stones (); ++stone_id) {
    if (matching.is_stone_matched (stone_id)) {
      fprintf (os, "%d ", matching.matched_city (stone_id));
    } else {
      fprintf (os, "-1 ");
    }
  }
  fprintf (os, "\n");

  route.for_each_edge ([&] (int from, int to) //
    { fprintf (os, "%d ", from); });
  fprintf (os, "%d\n***\n", route.dataset ().starting_city ());
}
