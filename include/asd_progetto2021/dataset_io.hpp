#pragma once
#include <asd_progetto2021/dataset.hpp>

#include <iomanip>

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

  auto stones = StoneIndex (num_stones, num_cities);
  for (int stone_id = 0; stone_id < num_stones; ++stone_id)
    CHECK (is >> stones.stone (stone_id).weight >> stones.stone (stone_id).energy);

  for (int stone_id = 0; stone_id < num_stones; ++stone_id) {
    int available_len = -1;
    CHECK (is >> available_len);

    for (int i = 0; i < available_len; ++i) {
      int city_id = -1;
      CHECK (is >> city_id);
      stones.store (stone_id, city_id);
    }
  }

  auto graph = CompleteSymmetricGraph (num_cities);
  for (int city_id = 1; city_id < num_cities; ++city_id) {
    for (int neighbour_id = 0; neighbour_id < city_id; ++neighbour_id) {
      CHECK (is >> graph.distance (city_id, neighbour_id));
    }
  }

  return Dataset (std::move (graph), //
    std::move (stones),
    Glove (glove_capacity, glove_resistance),
    starting_city,
    min_velocity,
    max_velocity);
}

inline auto write_output (std::ostream& os, Route const& route, StoneMatching const& matching) -> void
{
  auto const& dataset = route.dataset ();
  ASSERT (route.is_cycle ());
  ASSERT (route.size () == dataset.num_cities () + 1);

  auto const evaluation = evaluate (route, matching);
  os << std::fixed << std::setprecision (10) << evaluation.score << ' ';
  os << std::fixed << std::setprecision (10) << evaluation.energy << ' ';
  os << std::fixed << std::setprecision (10) << evaluation.time_elapsed << '\n';

  for (int i = 0; i < dataset.num_stones (); ++i)
    os << matching.matched_city (i) << ' ';
  os << '\n';

  route.for_each_vertex ([&os] (int curr) { os << curr << ' '; });
  os << "\n***\n";
}