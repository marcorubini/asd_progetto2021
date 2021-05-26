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
inline auto edge_index (int from, int to) -> int
{
  if (from < to)
    std::swap (from, to);
  ASSERT (from != to);
  return from * MAX_VERTICES + to;
}
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
    : _num_vertices (num_vertices),         //
      _num_stones (num_stones),             //
      _starting_city (starting_city),       //
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
    ASSERT (stone_id >= 0 && stone_id < num_vertices ());
    return _stones[stone_id];
  }
  auto stone_cities (int stone_id) const -> std::vector<int> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _availability[stone_id];
  }
  auto delta_velocity () const -> double
  {
    return max_velocity () - min_velocity ();
  }
  auto resistance_factor () const -> double
  {
    return delta_velocity () / glove_capacity ();
  }
  auto apply_resistance (double velocity, double weight) const -> double
  {
    ASSERT (velocity >= min_velocity ());
    ASSERT (velocity <= max_velocity ());
    return std::max (min_velocity (), velocity - weight * resistance_factor ());
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
      distances[edge_index (j, i)] = dist;
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
inline auto compute_travel_time (Dataset const& dataset, //
  std::vector<int> const& stones,                        //,
  std::vector<int> const& route) -> double
{
  ASSERT (stones.size () == dataset.num_vertices ());
  ASSERT (route.size () == dataset.num_vertices () + 1);
  ASSERT (route[0] == route.back ());
  ASSERT (route[0] == dataset.starting_city ());
  int glove_weight = 0.0;
  int curr = dataset.starting_city ();
  double velocity = dataset.max_velocity ();
  double travel = 0.0;
  for (int i = 0; i < static_cast<int> (route.size ()) - 1; ++i) {
    if (stones[curr] != -1) {
      glove_weight += dataset.stone (stones[curr]).weight;
      ASSERT (glove_weight <= dataset.glove_capacity ());
    }
    int next = route[i + 1];
    velocity = dataset.apply_resistance (velocity, glove_weight);
    travel += dataset.travel_time (curr, next, velocity);
    curr = next;
  }
  return travel;
}
inline auto compute_total_energy (Dataset const& dataset, //
  std::vector<int> const& stones,                         //
  std::vector<int> const& route) -> int
{
  ASSERT (stones.size () == dataset.num_vertices ());
  ASSERT (route.size () == dataset.num_vertices () + 1);
  ASSERT (route[0] == route.back ());
  ASSERT (route[0] == dataset.starting_city ());
  int energy = 0;
  for (int i = 0; i < static_cast<int> (route.size ()) - 1; ++i)
    if (stones[route[i]] != -1)
      energy += dataset.stone (stones[route[i]]).energy;
  return energy;
}
inline auto write_output (std::ostream& os, //
  Dataset const& dataset,
  std::vector<int> const& stones,
  std::vector<int> const& route) -> void
{
  ASSERT (stones.size () == dataset.num_vertices ());
  ASSERT (route.size () == dataset.num_vertices () + 1);
  ASSERT (route[0] == route.back ());
  ASSERT (route[0] == dataset.starting_city ());
  auto const travel = compute_travel_time (dataset, stones, route);
  auto const energy = compute_total_energy (dataset, stones, route);
  os << energy - travel * dataset.glove_resistance () << ' ';
  os << energy << ' ';
  os << travel << '\n';
  for (int i = 0; i < static_cast<int> (dataset.num_vertices ()); ++i)
    os << stones[i] << ' ';
  os << '\n';
  for (int i = 0; i < static_cast<int> (route.size ()); ++i)
    os << route[i] << ' ';
  os << '\n';
  os << "***\n";
}
#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
int main ()
{
  auto rng = std::mt19937 (std::random_device {}());
  auto const data = read_dataset (std::cin);
  auto route = std::vector<int> (data.num_vertices ());
  auto stones = std::vector<int> (data.num_vertices (), -1);
  std::iota (route.begin (), route.end (), 0);
  std::swap (route[0], route[data.starting_city ()]);
  std::shuffle (route.begin () + 1, route.end (), rng);
  route.push_back (data.starting_city ());
  write_output (std::cout, data, stones, route);
}
