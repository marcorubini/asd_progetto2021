#pragma once
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#ifdef EVAL

#  define ASSERT(...) (void)0

#else

#  define ASSERT(...)                                                                                        \
    do {                                                                                                     \
      if (!(__VA_ARGS__)) {                                                                                  \
        fprintf (stderr, "ASSERTION FAILED: %s\n", #__VA_ARGS__);                                            \
        fprintf (stderr, "LINE: %d FILE: %s\n", __LINE__, __FILE__);                                         \
        fprintf (stderr, "FUNCTION: %s\n", __FUNCTION__);                                                    \
        std::terminate ();                                                                                   \
      }                                                                                                      \
    } while (0)

#endif

constexpr auto MAX_VERTICES = 2000;
constexpr auto MAX_EDGES = (MAX_VERTICES * (MAX_VERTICES - 1)) / 2;
constexpr auto MAX_STONES = 10000;

constexpr auto MAX_GLOVE_CAPACITY = 10000000;
constexpr auto MAX_GLOVE_RESISTANCE = 5000.0;

constexpr auto MAX_STONE_WEIGHT = 100000;
constexpr auto MAX_STONE_ENERGY = 100000;

constexpr auto MAX_VELOCITY = 1000.0;
constexpr auto MAX_DISTANCE = 10000;

inline auto edge_index (int from, int to) -> int
{
  if (from < to)
    std::swap (from, to);
  ASSERT (from != to);
  return (from * (from - 1)) / 2 + to;
}

struct Stone
{
  int energy;
  int weight;

  bool operator== (Stone other) const
  {
    return energy == other.energy && weight == other.weight;
  }

  bool operator!= (Stone other) const
  {
    return energy != other.energy || weight != other.weight;
  }

  bool operator< (Stone other) const
  {
    if (energy != other.energy)
      return energy < other.energy;
    return weight < other.weight;
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
  std::vector<std::vector<int>> _stones_availability;
  std::vector<std::vector<int>> _stones_stored;

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
    : _num_vertices (num_vertices),                    //
      _num_stones (num_stones),                        //
      _starting_city (starting_city),                  //
      _glove_capacity (glove_capacity),                //
      _glove_resistance (glove_resistance),            //
      _vmin (vmin),                                    //
      _vmax (vmax),                                    //
      _distances (std::move (distances)),              //
      _stones (std::move (stones)),                    //
      _stones_availability (std::move (availability)), //
      _stones_stored (_num_vertices)
  {
    // Validate input
    ASSERT (_num_vertices >= 1 && _num_vertices <= MAX_VERTICES);
    ASSERT (_num_stones >= 0 && _num_stones <= MAX_STONES);
    ASSERT (_starting_city >= 0 && _starting_city < num_vertices);
    ASSERT (glove_capacity >= 0 && glove_capacity <= MAX_GLOVE_CAPACITY);
    ASSERT (vmin >= 0 && vmin <= vmax);
    ASSERT (vmax >= vmin && vmax <= MAX_VELOCITY);
    ASSERT (_distances.size () == MAX_EDGES);
    ASSERT (_stones.size () == num_stones);
    ASSERT (_stones_availability.size () == num_stones);

    int max_distance = 0;
    for (int i = 0; i < _num_vertices; ++i)
      for (int j = i + 1; j < _num_vertices; ++j)
        ASSERT (distance (i, j) >= 0 && distance (i, j) <= 10000);

    for (int i = 0; i < _num_stones; ++i)
      for (int c : cities_containing (i))
        ASSERT (c >= 0 && c < _num_vertices);

    for (auto& stone : _stones) {
      ASSERT (stone.weight >= 0 && stone.weight <= MAX_STONE_WEIGHT);
      ASSERT (stone.energy >= 0 && stone.energy <= MAX_STONE_ENERGY);
    }

    for (int i = 0; i < _num_stones; ++i)
      for (int c : cities_containing (i))
        _stones_stored[c].push_back (i);

    for (int i = 0; i < _num_vertices; ++i)
      std::sort (_stones_stored[i].begin (), _stones_stored[i].end ());
  }

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

  auto delta_velocity () const -> double
  {
    return max_velocity () - min_velocity ();
  }

  auto slowdown_factor () const -> double
  {
    return delta_velocity () / glove_capacity ();
  }

  auto compute_velocity (double weight) const -> double
  {
    ASSERT (weight >= 0);
    ASSERT (weight <= glove_capacity ());
    return std::max (min_velocity (), max_velocity () - weight * slowdown_factor ());
  }

  auto distance (int from, int to) const -> int
  {
    ASSERT (from != to);
    ASSERT (from >= 0 && from < num_vertices ());
    ASSERT (to >= 0 && to < num_vertices ());

    return _distances[edge_index (from, to)];
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
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones[stone_id];
  }

  auto stone_profit (int stone_id, int time, int weight, double velocity, std::vector<int> const& route) const -> double
  {
    double travel = 0;
    while (time + 1 < static_cast<int> (route.size ())) {
      weight += stone (stone_id).weight;
      velocity = compute_velocity (weight);
      travel += travel_time (route[time], route[time + 1], velocity) * glove_resistance ();
      time++;
    }

    return stone (stone_id).energy - travel * glove_resistance ();
  }

  auto cities_containing (int stone_id) const -> std::vector<int> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones_availability[stone_id];
  }

  auto stones_stored_at (int city_id) const -> std::vector<int> const&
  {
    ASSERT (city_id >= 0 && city_id < num_vertices ());
    return _stones_stored[city_id];
  }

  auto city_stores (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_vertices ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    auto& stored = stones_stored_at (city_id);
    return std::binary_search (stored.begin (), stored.end (), stone_id);
  }
};

inline auto read_dataset (std::istream& is) -> Dataset
{
  int num_vertices = -1;
  int starting_city = -1;

  is >> num_vertices >> starting_city;

  int num_stones = -1;
  int glove_capacity = -1;
  double glove_resistance = -1;
  double min_velocity = -1;
  double max_velocity = -1;

  is >> num_stones >> glove_capacity >> glove_resistance >> min_velocity >> max_velocity;

  auto stones = std::vector<Stone> (num_stones);
  for (auto& stone : stones)
    is >> stone.weight >> stone.energy;

  auto availability = std::vector<std::vector<int>> (num_stones);
  for (int i = 0; i < num_stones; ++i) {
    int len = -1;
    is >> len;
    availability[i].resize (len);
    for (int j = 0; j < len; ++j)
      is >> availability[i][j];
  }

  auto distances = std::vector<int> (MAX_EDGES);
  for (int i = 1; i < num_vertices; ++i)
    for (int j = 0; j < i; ++j)
      is >> distances[edge_index (i, j)];

  return Dataset (num_vertices,
    num_stones,
    starting_city,
    glove_capacity,
    glove_resistance,
    min_velocity,
    max_velocity,
    std::move (distances),
    std::move (stones),
    std::move (availability));
}

struct Evaluation
{
  int glove_energy;
  double time_elapsed;
  double score;
};

inline auto evaluate (Dataset const& dataset, //
  std::vector<int> const& stones,             //
  std::vector<int> const& route) -> Evaluation
{
  ASSERT (stones.size () == dataset.num_stones ());
  ASSERT (route.size () == dataset.num_vertices () + 1);
  ASSERT (route[0] == route.back ());
  ASSERT (route[0] == dataset.starting_city ());

  auto stone_taken = std::vector<int> (dataset.num_vertices (), -1);
  for (int i = 0; i < (int)stones.size (); ++i) {
    if (stones[i] != -1) {
      ASSERT (stones[i] >= 0 && stones[i] < dataset.num_vertices ());
      ASSERT (stone_taken[stones[i]] == -1);
      ASSERT (dataset.city_stores (stones[i], i));
      stone_taken[stones[i]] = i;
    }
  }

  int weight = 0;
  int energy = 0;
  int curr_vertex = dataset.starting_city ();

  double velocity = dataset.max_velocity ();
  double total_time = 0.0;

  for (int i = 0; i + 1 < (int)route.size (); ++i) {
    if (stone_taken[curr_vertex] != -1) {
      weight += dataset.stone (stone_taken[curr_vertex]).weight;
      energy += dataset.stone (stone_taken[curr_vertex]).energy;
      ASSERT (weight <= dataset.glove_capacity ());
    }

    velocity = dataset.compute_velocity (weight);
    total_time += dataset.travel_time (curr_vertex, route[i + 1], velocity);
    curr_vertex = route[i + 1];
  }

  return Evaluation {energy, total_time, energy - total_time * dataset.glove_resistance ()};
}

inline auto write_output (std::ostream& os, //
  Dataset const& dataset,
  std::vector<int> const& stones,
  std::vector<int> const& route) -> void
{
  ASSERT (stones.size () == dataset.num_stones ());
  ASSERT (route.size () == dataset.num_vertices () + 1);
  ASSERT (route[0] == route.back ());
  ASSERT (route[0] == dataset.starting_city ());

  auto evaluated = evaluate (dataset, stones, route);

  os << std::fixed << std::setprecision (10) << evaluated.score << ' ';
  os << std::fixed << std::setprecision (10) << evaluated.glove_energy << ' ';
  os << std::fixed << std::setprecision (10) << evaluated.time_elapsed << '\n';

  for (int i = 0; i < static_cast<int> (dataset.num_stones ()); ++i)
    os << stones[i] << ' ';
  os << '\n';

  for (int i = 0; i < static_cast<int> (route.size ()); ++i)
    os << route[i] << ' ';
  os << '\n';

  os << "***\n";
}