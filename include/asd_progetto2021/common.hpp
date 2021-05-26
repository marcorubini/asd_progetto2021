#pragma once
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#define ASSERT(...)                                                                                          \
  do {                                                                                                       \
    if (!(__VA_ARGS__)) {                                                                                    \
      fprintf (stderr, "ASSERTION FAILED: %s\n", #__VA_ARGS__);                                              \
      fprintf (stderr, "LINE: %d FILE: %s\n", __LINE__, __FILE__);                                           \
      fprintf (stderr, "FUNCTION: %s\n", __FUNCTION__);                                                      \
      std::terminate ();                                                                                     \
    }                                                                                                        \
  } while (0)

constexpr auto MAX_VERTICES = 2000;
constexpr auto MAX_STONES = 10000;
constexpr auto MAX_GLOVE_CAPACITY = 10000000;
constexpr auto MAX_GLOVE_RESISTANCE = 5000.0;
constexpr auto MAX_VELOCITY = 1000.0;
constexpr auto MAX_STONE_WEIGHT = 100000;
constexpr auto MAX_DISTANCE = 100000;

struct Edge
{
  int from;
  int to;

  constexpr bool operator== (Edge other) const
  {
    return from == other.from && to == other.to;
  }
  constexpr bool operator!= (Edge other) const
  {
    return from != other.from || to != other.to;
  }
};

constexpr auto canonical (Edge edge) -> Edge
{
  if (edge.from < edge.to)
    return edge;
  else
    return {edge.to, edge.from};
}

constexpr auto to_index (Edge edge) -> int
{
  edge = canonical (edge);
  return edge.from * MAX_VERTICES + edge.to;
}

// =====

struct Stone
{
  int energy;
  int weight;

  constexpr bool operator== (Stone other) const
  {
    return energy == other.energy && weight == other.weight;
  }

  constexpr bool operator!= (Stone other) const
  {
    return energy != other.energy || weight != other.weight;
  }
};

struct Dataset
{
private:
  int _num_vertices;
  int _num_stones;

  int _starting_city;
  int _glove_capacity;
  double _glove_resistance;
  double _vmin;
  double _vmax;

  std::vector<int> _distances;
  std::vector<Stone> _stones;
  std::vector<std::vector<int>> _availability;

public:
  Dataset (int num_vertices,    //
    int num_stones,             //
    int starting_city,          //
    int glove_capacity,         //
    double glove_resistance,    //
    double vmin,                //
    double vmax,                //
    std::vector<int> distances, //
    std::vector<Stone> stones,  //
    std::vector<std::vector<int>> availability)
    : _starting_city (starting_city),       //
      _glove_capacity (glove_capacity),     //
      _glove_resistance (glove_resistance), //
      _vmin (vmin),                         //
      _vmax (vmax),                         //
      _distances (std::move (distances)),   //
      _stones (std::move (stones)),         //
      _availability (std::move (availability))
  {}

  auto num_stones () const -> int
  {
    return _num_stones;
  }

  auto num_vertices () const -> int
  {
    return _num_vertices;
  }

  auto starting_city () const -> int
  {
    return _starting_city;
  }

  auto glove_capacity () const -> int
  {
    return _glove_capacity;
  }

  auto min_velocity () const -> double
  {
    return _vmin;
  }

  auto max_velocity () const -> double
  {
    return _vmax;
  }

  auto glove_resistance () const -> double
  {
    return _glove_resistance;
  }

  auto distance (int from, int to) const -> int
  {
    ASSERT (from != to);
    ASSERT (from >= 0 && from < num_vertices ());
    ASSERT (to >= 0 && to < num_vertices ());

    return _distances[to_index (Edge {from, to})];
  }

  auto travel_time (int from, int to, double velocity) const -> double
  {
    ASSERT (from != to);
    ASSERT (from >= 0 && from < num_vertices ());
    ASSERT (to >= 0 && to < num_vertices ());
    ASSERT (velocity >= min_velocity ());
    ASSERT (velocity <= max_velocity ());

    return distance (from, to) / velocity;
  }

  auto stone (int stone_id) const -> Stone
  {
    ASSERT (stone_id >= 0 && stone_id < num_vertices ());
    return _stones[stone_id];
  }

  auto stone_cities (int stone_id) const -> std::vector<int> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _availability[stone_id];
  }
};

inline auto read_dataset (std::istream& is) -> Dataset
{
  int num_vertices = -1;
  int starting_city = -1;

  is >> num_vertices >> starting_city;
  ASSERT (num_vertices >= 1 && num_vertices <= MAX_VERTICES);
  ASSERT (starting_city >= 0 && starting_city < num_vertices);

  int num_stones = -1;
  int glove_capacity = -1;
  double glove_resistance = -1;
  double min_velocity = -1;
  double max_velocity = -1;
  is >> num_stones >> glove_capacity >> glove_resistance >> min_velocity >> max_velocity;
  ASSERT (num_stones >= 0 && num_stones <= MAX_STONES);
  ASSERT (glove_capacity >= 0 && glove_capacity <= MAX_GLOVE_CAPACITY);
  ASSERT (glove_resistance >= 0 && glove_resistance <= MAX_GLOVE_RESISTANCE);
  ASSERT (min_velocity >= 0 && min_velocity <= max_velocity);
  ASSERT (max_velocity >= 0 && max_velocity <= MAX_VELOCITY);

  auto stones = std::vector<Stone> (num_stones);
  for (auto& stone : stones)
    is >> stone.energy >> stone.weight;

  auto availability = std::vector<std::vector<int>> (num_stones);
  for (int i = 0; i < num_stones; ++i) {
    int len = -1;
    is >> len;
    ASSERT (len >= 0 && len <= num_vertices);
    for (int j = 0; j < len; ++j) {
      int curr;
      is >> curr;
      ASSERT (curr >= 0 && curr < num_vertices);
      availability[i].push_back (curr);
    }
  }

  auto distances = std::vector<int> ((MAX_VERTICES * (MAX_VERTICES - 1)) / 2);
  for (int i = 1; i < num_vertices; ++i) {
    for (int j = 0; j < i; ++j) {
      int dist = -1;
      is >> dist;
      ASSERT (dist >= 0 && dist <= MAX_DISTANCE);
      distances[to_index (Edge {j, i})] = dist;
    }
  }

  return Dataset (num_vertices,
    num_stones,
    starting_city,
    glove_capacity,
    glove_resistance,
    min_velocity, //
    max_velocity,
    std::move (distances),
    std::move (stones),
    std::move (availability));
}

template<class Route, class StoneMap>
inline auto write_output (std::ostream& os,
  int num_vertices,    //
  double final_energy, //
  double glove_energy, //
  double travel_time,  //
  StoneMap&& stones,   //
  Route&& route) -> void
{
  os << std::fixed << std::setprecision (10) << final_energy << ' ' //
     << glove_energy << ' ' << travel_time << '\n';

  for (int i = 0; i < num_vertices; ++i)
    os << stones[i] << ' ';
  os << '\n';

  for (int t : route)
    os << t;
  os << '\n';

  os << "***\n";
}

#if 0
inline auto compute_travel_time (Dataset const& dataset, std::vector<int> const& stones, std::vector<int> const& route) -> void
{
  int glove_weight = 0.0;
  int glove_capacity = dataset.glove_capacity ();
  int curr = dataset.starting_city ();
  double velocity = dataset.max_velocity ();
}

inline auto evaluate (Dataset const& dataset, std::vector<int> const& stones, std::vector<int> const& route)
{
  int glove_remaining_capacity = dataset.glove_capacity ();
  int glove_energy = 0;

  // compute total glove energy
  for (int i = 0; i < dataset.num_vertices (); ++i) {
    int s = stones[i];
    if (s != -1) {
      auto const stone = dataset.stone (i);
      ASSERT (stone.weight <= glove_remaining_capacity);
      glove_remaining_capacity -= stone.weight;
      glove_energy += stone.energy;
    }
  }

  // compute total travel time
  int curr_vertex = dataset.starting_city ();
  int curr_weight = 0;
  double curr_velocity = dataset.max_velocity ();
  for (int i = 1; i < (int)route.size (); ++i) {
    if (stones[i] != -1)

      curr_velocity -=
        curr_weight * (dataset.max_velocity () - dataset.min_velocity ()) / (double)dataset.glove_capacity ();
    curr_velocity = std::max (curr_velocity, dataset.min_velocity ());
    auto dist = dataset.travel_time (curr_vertex, route[i], curr_velocity);
  }
}
#endif