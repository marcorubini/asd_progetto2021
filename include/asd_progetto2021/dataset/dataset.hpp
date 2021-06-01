#pragma once
#include <asd_progetto2021/dataset/glove.hpp>
#include <asd_progetto2021/dataset/graph.hpp>
#include <asd_progetto2021/dataset/limits.hpp>
#include <asd_progetto2021/dataset/stone_index.hpp>
#include <asd_progetto2021/utilities/assert.hpp>

// Aggregates information about all input data.
struct Dataset
{
  using index_type = typename StoneIndex::index_type;

private:
  CompleteSymmetricGraph _graph;
  StoneIndex _stones;
  Glove _glove;

  int _starting_city;
  double _min_velocity;
  double _max_velocity;

public:
  Dataset (CompleteSymmetricGraph graph, //
    StoneIndex stones,                   //
    Glove glove,                         //
    int starting_city,                   //
    double min_velocity,                 //
    double max_velocity)
    : _graph (std::move (graph)),     //
      _stones (std::move (stones)),   //
      _glove (glove),                 //
      _starting_city (starting_city), //
      _min_velocity (min_velocity),   //
      _max_velocity (max_velocity)
  {
    ASSERT (_starting_city >= 0 && _starting_city < _graph.num_cities ());
    ASSERT (_min_velocity >= 0.0 && _min_velocity <= _max_velocity);
    ASSERT (_max_velocity >= _min_velocity && _max_velocity <= MAX_VELOCITY);
  }

  Dataset (Dataset const&) = delete;
  Dataset (Dataset&&) = default;

  auto num_cities () const -> int
  {
    return _graph.num_cities ();
  }

  auto num_stones () const -> int
  {
    return _stones.num_stones ();
  }

  auto glove_capacity () const -> int
  {
    return _glove.capacity;
  }

  auto glove_resistance () const -> double
  {
    return _glove.resistance;
  }

  auto min_velocity () const -> double
  {
    return _min_velocity;
  }

  auto max_velocity () const -> double
  {
    return _max_velocity;
  }

  auto starting_city () const -> int
  {
    return _starting_city;
  }

  // --- Graph information

  auto graph () const -> CompleteSymmetricGraph const&
  {
    return _graph;
  }

  auto distance (int from, int to) const -> int
  {
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return _graph.distance (from, to);
  }

  auto travel_time (int length, int weight) const -> double
  {
    ASSERT (-glove_capacity () <= weight && glove_capacity () >= weight);
    if (weight < 0)
      return -travel_time (length, -weight);

    auto const delta_velocity = max_velocity () - min_velocity ();
    auto const slowness_factor = delta_velocity / glove_capacity ();
    auto const current_velocity = std::max (min_velocity (), max_velocity () - weight * slowness_factor);
    return length / current_velocity;
  }

  auto travel_time (int from, int to, int weight) const -> double
  {
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    ASSERT (-glove_capacity () <= weight && glove_capacity () >= weight);
    return travel_time (distance (from, to), weight);
  }

  auto travel_cost (int length, int weight) const -> double
  {
    ASSERT (-glove_capacity () <= weight && glove_capacity () >= weight);
    return travel_time (length, weight) * glove_resistance ();
  }

  auto travel_cost (int from, int to, int weight) const -> double
  {
    ASSERT (-glove_capacity () <= weight && glove_capacity () >= weight);
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return travel_time (from, to, weight) * glove_resistance ();
  }

  // --- Stone information

  auto stones () const -> StoneIndex const&
  {
    return _stones;
  }

  auto stone (int stone_id) const -> Stone
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones.stone (stone_id);
  }

  auto cities_with_stone (int stone_id) const -> std::vector<index_type> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones.cities_with_stone (stone_id);
  }

  auto city_has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones.city_has_stone (city_id, stone_id);
  }

  // score

  auto final_score (int final_energy, double travel_time) const -> double
  {
    return final_energy - glove_resistance () * travel_time;
  }
};