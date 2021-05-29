#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
#pragma GCC optimize("Ofast")
#include <fstream>
#include <iostream>
#include <stdexcept>
#ifdef EVAL
#  define ASSERT(...) (void)0
#else
#  define ASSERT(...)                                                                                        \
    do {                                                                                                     \
      if (!(__VA_ARGS__)) {                                                                                  \
        fprintf (stderr, "ASSERTION FAILED: %s\n", #__VA_ARGS__);                                            \
        fprintf (stderr, "LINE: %d FILE: %s FUNCTION: %s\n", __LINE__, __FILE__, __FUNCTION__);              \
        std::terminate ();                                                                                   \
      }                                                                                                      \
    } while (0)
#endif
#define CHECK(...)                                                                                           \
  do {                                                                                                       \
    if (!(__VA_ARGS__)) {                                                                                    \
      fprintf (stderr, "CHECK FAILED: %s\n", #__VA_ARGS__);                                                  \
      fprintf (stderr, "LINE: %d FILE: %s FUNCTION: %s\n", __LINE__, __FILE__, __FUNCTION__);                \
      std::terminate ();                                                                                     \
    }                                                                                                        \
  } while (0)
constexpr auto MAX_CITIES = 2000;
constexpr auto MAX_EDGES = (MAX_CITIES * (MAX_CITIES + 1)) / 2;
constexpr auto MAX_STONES = 10000;
constexpr auto MAX_GLOVE_CAPACITY = 10000000;
constexpr auto MAX_GLOVE_RESISTANCE = 5000.0;
constexpr auto MAX_STONE_WEIGHT = 100000;
constexpr auto MAX_STONE_ENERGY = 100000;
constexpr auto MAX_VELOCITY = 1000.0;
constexpr auto MAX_DISTANCE = 10000;
struct Glove
{
  int capacity {-1};
  double resistance {-1};
  Glove () = default;
  Glove (int capacity, double resistance) : capacity (capacity), resistance (resistance)
  {
    ASSERT (capacity >= 0 && capacity <= MAX_GLOVE_CAPACITY);
    ASSERT (resistance >= 0 && resistance <= MAX_GLOVE_RESISTANCE);
  }
};
#include <vector>
struct CompleteSymmetricGraph
{
private:
  std::vector<int> _distances = std::vector<int> (MAX_EDGES);
  int _num_cities = 0;
public:
  CompleteSymmetricGraph (int num_cities) : _num_cities (num_cities)
  {
    ASSERT (num_cities >= 1 && num_cities <= MAX_CITIES);
  }
  static auto index (int from, int to) -> int
  {
    if (from < to)
      std::swap (from, to);
    return (from * (from + 1)) / 2 + to;
  }
  auto distance (int from, int to) const& -> int const&
  {
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return _distances[index (from, to)];
  }
  auto distance (int from, int to) & -> int&
  {
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return _distances[index (from, to)];
  }
  auto num_cities () const -> int
  {
    return _num_cities;
  }
};
struct Stone
{
  int weight {-1};
  int energy {-1};
  Stone () = default;
  Stone (int weight, int energy) : weight (weight), energy (energy)
  {
    ASSERT (weight >= 1 && weight <= MAX_STONE_WEIGHT);
    ASSERT (energy >= 1 && energy <= MAX_STONE_ENERGY);
  }
  bool operator== (Stone other) const
  {
    return weight == other.weight && energy == other.energy;
  }
  bool operator!= (Stone other) const
  {
    return weight != other.weight || energy != other.energy;
  }
  bool operator< (Stone other) const
  {
    return weight < other.weight || (weight == other.weight && energy < other.energy);
  }
};
// Returns true if lhs is better than rhs.
inline auto pareto_compare (Stone lhs, Stone rhs) -> bool
{
  if (lhs.energy != rhs.energy)
    return lhs.energy > rhs.energy && lhs.weight <= rhs.weight;
  return lhs.weight < rhs.weight;
}
#include <algorithm>
#include <array>
#include <vector>
// Stores information about all stones and in which cities they can be found.
struct StoneIndex
{
private:
  std::array<Stone, MAX_STONES> _stones {};
  std::vector<std::vector<int>> _stones_per_city {};
  std::vector<std::vector<int>> _cities_with_stone {};
  std::vector<std::pair<int, int>> _stone_city_edges {};
  int _num_stones {};
  int _num_cities {};
public:
  StoneIndex (int num_stones, int num_cities) //
    : _stones_per_city (num_cities),          //
      _cities_with_stone (num_stones),        //
      _num_stones (num_stones),               //
      _num_cities (num_cities)                //
  {
    ASSERT (num_stones >= 0 && num_stones <= MAX_STONES);
    ASSERT (num_cities >= 0 && num_cities <= MAX_CITIES);
  }
  auto num_stones () const -> int
  {
    return _num_stones;
  }
  auto num_cities () const -> int
  {
    return _num_cities;
  }
  auto num_edges () const -> int
  {
    return _stone_city_edges.size ();
  }
  auto edges () const -> std::vector<std::pair<int, int>> const&
  {
    return _stone_city_edges;
  }
  auto edge (int index) const -> std::pair<int, int>
  {
    ASSERT (index >= 0 && index < num_edges ());
    return _stone_city_edges[index];
  }
  auto begin () const -> std::array<Stone, MAX_STONES>::const_iterator
  {
    return _stones.begin ();
  }
  auto begin () -> std::array<Stone, MAX_STONES>::iterator
  {
    return _stones.begin ();
  }
  auto end () const -> std::array<Stone, MAX_STONES>::const_iterator
  {
    return begin () + num_stones ();
  }
  auto end () -> std::array<Stone, MAX_STONES>::iterator
  {
    return begin () + num_stones ();
  }
  auto operator[] (int pos) const -> Stone const&
  {
    ASSERT (pos >= 0 && pos < num_stones ());
    return _stones[pos];
  }
  auto operator[] (int pos) -> Stone&
  {
    ASSERT (pos >= 0 && pos < num_stones ());
    return _stones[pos];
  }
  auto stone (int stone_id) const -> Stone const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones[stone_id];
  }
  auto stone (int stone_id) -> Stone&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones[stone_id];
  }
  auto store (int stone_id, int city_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    ASSERT (city_id >= 0 && city_id < num_cities ());
    _stones_per_city[city_id].push_back (stone_id);
    _cities_with_stone[stone_id].push_back (city_id);
    _stone_city_edges.emplace_back (stone_id, city_id);
  }
  auto cities_with_stone (int stone_id) const -> std::vector<int> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _cities_with_stone[stone_id];
  }
  auto stones_at_city (int city_id) const -> std::vector<int> const&
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    return _stones_per_city[city_id];
  }
  auto sort () -> void
  {
    for (auto& vec : _stones_per_city)
      std::sort (vec.begin (), vec.end ());
    for (auto& vec : _cities_with_stone)
      std::sort (vec.begin (), vec.end ());
    std::sort (_stone_city_edges.begin (), _stone_city_edges.end ());
  }
  auto has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return std::binary_search (_stones_per_city[city_id].begin (), _stones_per_city[city_id].end (), stone_id);
  }
};
// Aggregates information about all input data.
struct Dataset
{
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
  auto num_stone_edges () const -> int
  {
    return _stones.num_edges ();
  }
  auto stone_edge (int index) const -> std::pair<int, int>
  {
    ASSERT (index >= 0 && index < num_stone_edges ());
    return _stones.edge (index);
  }
  auto stone_edges () const -> std::vector<std::pair<int, int>> const&
  {
    return _stones.edges ();
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
  auto has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    ASSERT (city_id >= 0 && city_id < num_cities ());
    return _stones.has_stone (city_id, stone_id);
  }
  auto stones_at_city (int city_id) const -> std::vector<int> const&
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    return _stones.stones_at_city (city_id);
  }
  auto cities_with_stone (int stone_id) const -> std::vector<int> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _stones.cities_with_stone (stone_id);
  }
  // score
  auto final_score (int final_energy, double travel_time) const -> double
  {
    return final_energy - glove_resistance () * travel_time;
  }
};
#include <functional>
struct StoneMatching
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  std::array<int, MAX_STONES> _matched_city;
  std::array<int, MAX_CITIES> _matched_stone;
  int _weight = 0;
  int _energy = 0;
public:
  StoneMatching (Dataset const& dataset) : _dataset (dataset)
  {
    std::fill (_matched_city.begin (), _matched_city.end (), -1);
    std::fill (_matched_stone.begin (), _matched_stone.end (), -1);
  }
  auto dataset () const -> Dataset const&
  {
    return _dataset.get ();
  }
  auto is_city_matched (int city_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return matched_stone (city_id) != -1;
  }
  auto is_stone_matched (int stone_id) const -> bool
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    return matched_city (stone_id) != -1;
  }
  auto matched_stone (int city_id) const -> int
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return _matched_stone[city_id];
  }
  auto matched_city (int stone_id) const -> int
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    return _matched_city[stone_id];
  }
  auto match (int stone_id, int city_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (!is_city_matched (city_id));
    ASSERT (!is_stone_matched (stone_id));
    _matched_city[stone_id] = city_id;
    _matched_stone[city_id] = stone_id;
    _weight += dataset ().stone (stone_id).weight;
    _energy += dataset ().stone (stone_id).energy;
  }
  auto unmatch_stone (int stone_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (is_stone_matched (stone_id));
    auto const city_id = matched_city (stone_id);
    _matched_city[stone_id] = -1;
    _matched_stone[city_id] = -1;
    _weight -= dataset ().stone (stone_id).weight;
    _energy -= dataset ().stone (stone_id).energy;
  }
  auto unmatch_city (int city_id) -> void
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (is_city_matched (city_id));
    auto const stone_id = matched_stone (city_id);
    _matched_stone[city_id] = -1;
    _matched_city[stone_id] = -1;
    _weight -= dataset ().stone (stone_id).weight;
    _energy -= dataset ().stone (stone_id).energy;
  }
  auto weight () const -> int
  {
    return _weight;
  }
  auto energy () const -> int
  {
    return _energy;
  }
};
#include <array>
template<unsigned Capacity>
struct FenwickTree
{
private:
  std::array<int, Capacity> _sum {};
  int _size {};
public:
  FenwickTree () = default;
  auto size () const -> int
  {
    return _size;
  }
  auto prefix_sum (int last) const -> int
  {
    int sum = 0;
    while (last >= 0) {
      sum += _sum[last];
      last = (last & (last + 1)) - 1;
    }
    return sum;
  }
  auto sum (int first, int last) const -> int
  {
    return prefix_sum (last) - prefix_sum (first - 1);
  }
  auto add (int index, int value) -> void
  {
    while (index < Capacity) {
      _sum[index] += value;
      index = index | (index + 1);
    }
  }
};
#include <algorithm>
#include <array>
#include <functional>
struct SimpleRoute
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  std::array<int, MAX_CITIES> _route {};
  std::array<int, MAX_CITIES> _visited_index {};
  int _total_distance {};
public:
  SimpleRoute (Dataset const& dataset) //
    : _dataset (dataset)
  {
    _route[0] = dataset.starting_city ();
    _visited_index[0] = _route[0];
    int size = 1;
    for (int i = 0; i < dataset.num_cities (); ++i) {
      if (i == _route[0])
        continue;
      _route[size] = i;
      _visited_index[i] = size;
      _total_distance += dataset.distance (_route[size - 1], i);
      ++size;
    }
    _total_distance += dataset.distance (_route[size - 1], _route[0]);
  }
  auto dataset () const -> Dataset const&
  {
    return _dataset.get ();
  }
  auto length () const -> int
  {
    return _total_distance;
  }
  auto city_index (int city_id) const -> int
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return _visited_index[city_id];
  }
  auto at (int index) const -> int
  {
    ASSERT (index >= -1 && index <= dataset ().num_cities ());
    if (index == dataset ().num_cities ())
      return _route[0];
    if (index == -1)
      return _route[0];
    return _route[index];
  }
  auto swap_profit (int idx1, int idx2) const -> int
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());
    if (idx1 == idx2)
      return 0;
    auto const c1 = at (idx1 - 1);
    auto const c2 = at (idx1);
    auto const c3 = at (idx1 + 1);
    auto const c4 = at (idx2 - 1);
    auto const c5 = at (idx2);
    auto const c6 = at (idx2 + 1);
    auto const l1 = dataset ().distance (c1, c2);
    auto const l2 = dataset ().distance (c2, c3);
    auto const l3 = dataset ().distance (c4, c5);
    auto const l4 = dataset ().distance (c5, c6);
    auto const L1 = dataset ().distance (c1, c5);
    auto const L2 = dataset ().distance (c5, c3);
    auto const L3 = dataset ().distance (c4, c2);
    auto const L4 = dataset ().distance (c2, c6);
    if (idx1 + 1 == idx2) {
      return L1 - l1 + L4 - l4;
    } else {
      return L1 + L2 + L3 + L4 - l1 - l2 - l3 - l4;
    }
  }
  auto swap (int idx1, int idx2) -> void
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());
    if (idx1 == idx2)
      return;
    auto const profit = swap_profit (idx1, idx2);
    std::swap (_route[idx1], _route[idx2]);
    std::swap (_visited_index[_route[idx1]], _visited_index[_route[idx2]]);
    _total_distance += profit;
  }
  auto reverse_profit (int idx1, int idx2) const -> int
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());
    if (idx1 == idx2)
      return 0;
    auto const c1 = at (idx1 - 1);
    auto const c2 = at (idx1);
    auto const c3 = at (idx2);
    auto const c4 = at (idx2 + 1);
    auto const l1 = dataset ().distance (c1, c2);
    auto const l2 = dataset ().distance (c3, c4);
    auto const L1 = dataset ().distance (c1, c3);
    auto const L2 = dataset ().distance (c2, c4);
    return L1 + L2 - l1 - l2;
  }
  auto reverse (int idx1, int idx2) -> void
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());
    if (idx1 == idx2)
      return;
    auto const profit = reverse_profit (idx1, idx2);
    std::reverse (_route.begin () + idx1, _route.begin () + idx2 + 1);
    for (int i = idx1; i <= idx2; ++i)
      _visited_index[_route[i]] = i;
    _total_distance += profit;
  }
  template<class Fn>
  auto for_each_edge (Fn fn) const -> void
  {
    for (int i = 0; i < dataset ().num_cities (); ++i)
      fn (at (i), at (i + 1));
  }
  template<class Fn>
  auto for_each_vertex (Fn fn) const -> void
  {
    for (int i = 0; i < dataset ().num_cities (); ++i)
      fn (at (i));
  }
  auto subroute_length (int idx1, int idx2) const -> int
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 <= dataset ().num_cities ());
    int tot = 0;
    while (idx1 < idx2) {
      tot += dataset ().distance (at (idx1), at (idx1 + 1));
      ++idx1;
    }
    return tot;
  }
  auto prefix_length (int last_idx) const -> int
  {
    return subroute_length (0, last_idx);
  }
  auto suffix_length (int first_idx) const -> int
  {
    return subroute_length (first_idx, dataset ().num_cities ());
  }
};
inline auto solve_baseline (Dataset const& dataset) -> std::pair<SimpleRoute, StoneMatching>
{
  // Compute a greedy tour
  auto tour = [&] () {
    auto tour = SimpleRoute (dataset);
    for (int i = 1; i < dataset.num_cities (); ++i) {
      int curr_best = -1;
      int curr_dist = 1u << 30;
      for (int j = 0; j < dataset.num_cities (); ++j) {
        if (tour.city_index (j) >= i && dataset.distance (j, tour.at (i - 1)) < curr_dist) {
          curr_dist = dataset.distance (j, tour.at (i - 1));
          curr_best = j;
        }
      }
      tour.swap (i, tour.city_index (curr_best));
    }
    return tour;
  }();
  // Compute a greedy matching
  auto matching = [&] () {
    auto matching = StoneMatching (dataset);
    auto edges = dataset.stone_edges ();
    std::sort (edges.begin (), //
      edges.end (),            //
      [&] (std::pair<int, int> x, std::pair<int, int> y) {
        return dataset.stone (x.first).energy > dataset.stone (y.first).energy;
      });
    for (auto edge : edges) {
      auto stone_id = edge.first;
      auto city_id = edge.second;
      if (!matching.is_stone_matched (stone_id) && !matching.is_city_matched (city_id)) {
        auto weight = dataset.stone (stone_id).weight;
        auto energy = dataset.stone (stone_id).energy;
        if (matching.weight () + weight <= dataset.glove_capacity ()) {
          matching.match (stone_id, city_id);
        }
      }
    }
    return matching;
  }();
  return {std::move (tour), std::move (matching)};
}
#include <algorithm>
#include <random>
// Picks random (stone, city) pairs, then permutes the city order.
inline auto solve_random (Dataset const& dataset) -> std::pair<SimpleRoute, StoneMatching>
{
  auto route = SimpleRoute (dataset);
  auto matching = StoneMatching (dataset);
  auto rng = std::mt19937 (std::random_device {}());
  auto edges = dataset.stone_edges ();
  std::shuffle (edges.begin (), edges.end (), rng);
  for (auto e : edges) {
    auto stone_id = e.first;
    auto city_id = e.second;
    auto weight = dataset.stone (stone_id).weight;
    if (!matching.is_stone_matched (stone_id) && !matching.is_city_matched (city_id)) {
      if (weight + matching.weight () <= dataset.glove_capacity ()) {
        matching.match (stone_id, city_id);
      }
    }
  }
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  std::shuffle (indices.begin (), indices.end (), rng);
  int curr = 1;
  for (int i : indices)
    if (i != route.at (0))
      route.swap (curr++, route.city_index (i));
  return {std::move (route), std::move (matching)};
}
#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>
struct VisitSet
{
  std::vector<int> v;
  int v_id = 1;
  VisitSet () = default;
  VisitSet (int N) : v (N)
  {}
  void resize (int N)
  {
    v.assign (N, v_id);
  }
  void reset ()
  {
    v_id++;
  }
  void visit (int x)
  {
    v[x] = v_id;
  }
  void unvisit (int x)
  {
    v[x] = -1;
  }
  bool visited (int x) const
  {
    return v[x] == v_id;
  }
};
template<class T>
struct DoubleBuffer
{
  std::vector<T> buf[2];
  void push (const T& x)
  {
    buf[1].push_back (x);
  }
  T& back ()
  {
    if (buf[0].size () == 0)
      swap (buf[0], buf[1]);
    return buf[0].back ();
  }
  void pop ()
  {
    buf[0].pop_back ();
  }
  bool empty () const
  {
    return buf[0].size () == 0 && buf[1].size () == 0;
  }
  void clear ()
  {
    buf[0].clear ();
    buf[1].clear ();
  }
};
struct Dinic
{
  using pii = std::pair<int, int>;
  std::vector<std::tuple<int, int, int, int>> edges;
  std::vector<std::vector<int>> adj;
  VisitSet vis;
  std::vector<int> level;
  std::vector<int> next_edge;
  DoubleBuffer<int> buf;
  Dinic (int N) : adj (N), vis (N), level (N), next_edge (N)
  {}
  Dinic () = default;
  void resize (int N)
  {
    adj.assign (N, {});
    vis.resize (N);
    level.resize (N);
    next_edge.resize (N);
  }
  void clear ()
  {
    edges.clear ();
    buf.clear ();
    adj.clear ();
  }
  void add_edge (int from, int to, int capacity)
  {
    edges.push_back ({from, to, capacity, 0});
    edges.push_back ({to, from, 0, 0});
    adj[from].push_back (edges.size () - 2);
    adj[to].push_back (edges.size () - 1);
  }
  bool bfs (int src, int sink)
  {
    vis.reset ();
    buf.clear ();
    vis.visit (src);
    level[src] = 0;
    buf.push (src);
    while (!buf.empty ()) {
      auto curr = buf.back ();
      buf.pop ();
      for (int edge_id : adj[curr]) {
        int from, to, cap, flow;
        std::tie (from, to, cap, flow) = edges[edge_id];
        if (cap - flow > 0 && !vis.visited (to)) {
          vis.visit (to);
          level[to] = level[curr] + 1;
          buf.push (to);
        }
      }
    }
    return vis.visited (sink);
  }
  void augment (int edge_id, int bottleneck)
  {
    std::get<3> (edges[edge_id]) += bottleneck;
    std::get<3> (edges[edge_id ^ 1]) -= bottleneck;
  }
  int dfs (int curr, int sink, int bottleneck)
  {
    if (curr == sink)
      return bottleneck;
    const int num_edges = adj[curr].size ();
    for (; next_edge[curr] < num_edges; ++next_edge[curr]) {
      int edge_id = adj[curr][next_edge[curr]];
      int from, to, cap, flow;
      std::tie (from, to, cap, flow) = edges[edge_id];
      if (cap - flow > 0 && level[to] == level[from] + 1) {
        int pushed_flow = dfs (to, sink, std::min (bottleneck, cap - flow));
        if (pushed_flow > 0) {
          augment (edge_id, pushed_flow);
          return pushed_flow;
        }
      }
    }
    return 0;
  }
  std::pair<int, std::vector<std::tuple<int, int, int>>> solve (int src, int sink)
  {
    int max_flow = 0;
    while (bfs (src, sink)) {
      fill (begin (next_edge), end (next_edge), 0);
      for (int flow = dfs (src, sink, 1u << 30); flow != 0; flow = dfs (src, sink, 1u << 30)) {
        max_flow += flow;
      }
    }
    std::vector<std::tuple<int, int, int>> flow_edges;
    for (int i = 0; i < (int)edges.size (); i += 1) {
      int from, to, cap, flow;
      std::tie (from, to, cap, flow) = edges[i];
      if (flow > 0) {
        flow_edges.push_back ({from, to, flow});
      }
    }
    return {max_flow, std::move (flow_edges)};
  }
};
struct BipartiteMatching
{
  Dinic dinic;
  int size1;
  int size2;
  auto source () const -> int
  {
    return 0;
  }
  auto sink () const -> int
  {
    return 1;
  }
  auto left (int id) const -> int
  {
    return id + 2;
  }
  auto right (int id) const -> int
  {
    return size1 + id + 2;
  }
  BipartiteMatching (int size1, int size2) : dinic (size1 + size2 + 2), size1 (size1), size2 (size2)
  {
    for (int i = 0; i < size1; ++i)
      dinic.add_edge (source (), left (i), 1);
    for (int i = 0; i < size2; ++i)
      dinic.add_edge (right (i), sink (), 1);
  }
  auto add (int from, int to) -> void
  {
    dinic.add_edge (left (from), right (to), 1);
  }
  auto matching () -> std::vector<std::pair<int, int>>
  {
    auto flow = dinic.solve (source (), sink ());
    auto edges = std::vector<std::pair<int, int>> ();
    for (auto e : flow.second)
      if (std::get<0> (e) != source () && std::get<1> (e) != sink ())
        edges.emplace_back (std::get<0> (e) - 2, std::get<1> (e) - size1 - 2);
    return edges;
  }
};
struct Evaluation
{
  double score;
  int energy;
  double travel_time;
};
inline auto evaluate (SimpleRoute const& route, StoneMatching const& matching) -> Evaluation
{
  auto const& dataset = route.dataset ();
  int curr_weight = 0;
  double travel_time = 0;
  route.for_each_edge ([&] (int from, int to) {
    if (matching.is_city_matched (from))
      curr_weight += dataset.stone (matching.matched_stone (from)).weight;
    travel_time += dataset.travel_time (from, to, curr_weight);
  });
  return Evaluation {dataset.final_score (matching.energy (), travel_time), matching.energy (), travel_time};
}
#include <chrono>
#include <random>
inline auto tsp_bootstrap_greedy (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
{
  auto tour = SimpleRoute (dataset);
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  std::shuffle (indices.begin (), indices.end (), rng);
  for (int i = 1; i < dataset.num_cities (); ++i) {
    int curr_best = -1;
    int curr_dist = 1u << 30;
    for (int j : indices) {
      if (tour.city_index (j) < i)
        continue;
      if (dataset.distance (j, tour.at (i - 1)) >= curr_dist)
        continue;
      curr_dist = dataset.distance (j, tour.at (i - 1));
      curr_best = j;
    }
    tour.reverse (i, tour.city_index (curr_best));
  }
  return tour;
}
inline auto tsp_bootstrap_random (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
{
  auto tour = SimpleRoute (dataset);
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  std::shuffle (indices.begin (), indices.end (), rng);
  int size = 1;
  for (int i : indices)
    if (i != dataset.starting_city () && tour.city_index (i) >= size)
      tour.swap (size++, tour.city_index (i));
  return tour;
}
template<class Strategy>
inline auto tsp_bootstrap (Dataset const& dataset, Strategy strategy, std::mt19937& rng, int rounds) -> SimpleRoute
{
  auto result = SimpleRoute (dataset);
  while (rounds-- > 0) {
    auto curr = strategy (dataset, rng);
    if (curr.length () < result.length ())
      result = curr;
  }
  return result;
}
// ---
inline auto tsp_improve_all (SimpleRoute& route, std::mt19937& rng) -> int
{
  auto const& dataset = route.dataset ();
  auto indices = std::vector<int> (dataset.num_cities () - 1);
  std::iota (indices.begin (), indices.end (), 1);
  std::shuffle (indices.begin (), indices.end (), rng);
  int tot = 0;
  int threshold = 2;
  while (threshold > 0) {
    bool improved = false;
    for (int i : indices) {
      for (int j = i + 1; j < dataset.num_cities (); ++j) {
        auto change = route.reverse_profit (i, j);
        if (change <= -threshold) {
          tot += change;
          route.reverse (i, j);
          improved = true;
        }
      }
    }
    if (!improved)
      threshold /= 2;
  }
  return tot;
}
inline auto tsp_solve (Dataset const& dataset, std::mt19937& rng, std::chrono::milliseconds duration) -> SimpleRoute
{
  auto time_start = std::chrono::steady_clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now () - time_start);
  };
  auto result = SimpleRoute (dataset);
  auto greedy = tsp_bootstrap_greedy (dataset, rng);
  if (result.length () == greedy.length ())
    return greedy;
  result = greedy;
  while (elapsed () < duration) {
    for (auto strategy : {tsp_bootstrap_greedy}) {
      auto curr = tsp_bootstrap (dataset, strategy, rng, 2);
      tsp_improve_all (curr, rng);
      if (curr.length () < result.length ()) {
        result = curr;
      }
    }
  }
  return result;
}
#include <numeric>
#include <random>
using Matching = std::vector<std::pair<int, int>>;
using Edge = std::pair<int, int>;
inline auto greedy_select (Matching const& matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();
  auto result = StoneMatching (dataset);
  auto score = evaluate (route, result);
  for (auto e : matching) {
    if (result.is_city_matched (e.second))
      continue;
    if (result.is_stone_matched (e.first))
      continue;
    auto weight = dataset.stone (e.first).weight;
    if (weight + result.weight () > dataset.glove_capacity ())
      continue;
    result.match (e.first, e.second);
    auto new_score = evaluate (route, result);
    if (new_score.score < score.score) {
      result.unmatch_stone (e.first);
    } else {
      score = new_score;
    }
  }
  return result;
}
inline auto select_subset_heuristic_energy (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    return dataset.stone (e1.first).energy > dataset.stone (e2.first).energy;
  };
  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}
inline auto select_subset_heuristic_weight (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    return dataset.stone (e1.first).weight < dataset.stone (e2.first).weight;
  };
  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}
inline auto select_subset_heuristic_random (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto rng = std::mt19937 (std::random_device {}());
  auto const& dataset = route.dataset ();
  auto edges = dataset.stone_edges ();
  std::shuffle (edges.begin (), edges.end (), rng);
  return greedy_select (matching, route);
}
inline auto select_subset_heuristic_ratio (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    auto energy1 = dataset.stone (e1.first).energy;
    auto energy2 = dataset.stone (e2.first).energy;
    auto weight1 = dataset.stone (e1.first).weight;
    auto weight2 = dataset.stone (e2.first).weight;
    return energy1 / (weight1 + 1) > energy2 / (weight2 + 1);
  };
  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}
inline auto select_subset_heuristic_heavy (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    double energy1 = dataset.stone (e1.first).energy;
    double energy2 = dataset.stone (e2.first).energy;
    auto weight1 = dataset.stone (e1.first).weight;
    auto weight2 = dataset.stone (e2.first).weight;
    auto c1 = route.city_index (e1.second);
    auto c2 = route.city_index (e2.second);
    auto d1 = (dataset.num_cities () - c1);
    auto d2 = (dataset.num_cities () - c2);
    auto x = energy1 * 100 - d1 * weight1;
    auto y = energy2 * 100 - d2 * weight2;
    return x > y;
  };
  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}
// ---
inline auto make_matching_greedy1 (Dataset const& dataset, SimpleRoute const&) -> std::vector<std::pair<int, int>>
{
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    auto stone1 = dataset.stone (e1.first);
    auto stone2 = dataset.stone (e2.first);
    return stone1.energy / stone1.weight > stone2.energy / stone2.weight;
  };
  std::sort (edges.begin (), edges.end (), compare);
  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto result = std::vector<std::pair<int, int>> ();
  for (auto e : edges) {
    if (taken_stones[e.first])
      continue;
    if (taken_cities[e.second])
      continue;
    result.push_back (e);
    taken_stones[e.first] = true;
    taken_cities[e.second] = true;
  }
  return result;
}
inline auto make_matching_greedy2 (Dataset const& dataset, SimpleRoute const&) -> std::vector<std::pair<int, int>>
{
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    auto stone1 = dataset.stone (e1.first);
    auto stone2 = dataset.stone (e2.first);
    return stone1.weight < stone2.weight;
  };
  std::sort (edges.begin (), edges.end (), compare);
  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto result = std::vector<std::pair<int, int>> ();
  for (auto e : edges) {
    if (taken_stones[e.first])
      continue;
    if (taken_cities[e.second])
      continue;
    result.push_back (e);
    taken_stones[e.first] = true;
    taken_cities[e.second] = true;
  }
  return result;
}
inline auto make_matching_mcbm (Dataset const& dataset, SimpleRoute const& route) -> std::vector<std::pair<int, int>>
{
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto edges = std::vector<std::pair<int, int>> ();
  auto augment = [&] (int bound) {
    auto bip = BipartiteMatching (dataset.num_stones (), dataset.num_cities ());
    for (int i = dataset.num_cities () - 1; i >= 0; --i) {
      int city = route.at (i);
      if (taken_cities[city])
        continue;
      for (int stone_id : dataset.stones_at_city (city)) {
        if (taken_stones[stone_id])
          continue;
        int energy = dataset.stone (stone_id).energy;
        if (energy < bound)
          continue;
        bip.add (stone_id, city);
      }
    }
    for (auto e : bip.matching ()) {
      edges.push_back (e);
      taken_stones[e.first] = true;
      taken_cities[e.second] = true;
    }
  };
  auto bound = 0;
  for (auto e : dataset.stones ())
    bound = std::max (bound, e.energy);
  int full = std::min (dataset.num_cities (), dataset.num_stones ());
  while (bound > 0 && (int)edges.size () < full) {
    augment (bound);
    bound -= (bound / 200) + 1;
  }
  return edges;
}
inline auto make_matching_mcbm_weight (Dataset const& dataset, SimpleRoute const& route)
  -> std::vector<std::pair<int, int>>
{
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto edges = std::vector<std::pair<int, int>> ();
  auto augment = [&] (int bound) {
    auto bip = BipartiteMatching (dataset.num_stones (), dataset.num_cities ());
    for (int i = dataset.num_cities () - 1; i >= 0; --i) {
      int city = route.at (i);
      if (taken_cities[city])
        continue;
      for (int stone_id : dataset.stones_at_city (city)) {
        if (taken_stones[stone_id])
          continue;
        int weight = dataset.stone (stone_id).weight;
        if (weight > bound)
          continue;
        bip.add (stone_id, city);
      }
    }
    for (auto e : bip.matching ()) {
      edges.push_back (e);
      taken_stones[e.first] = true;
      taken_cities[e.second] = true;
    }
  };
  auto bound = (int)(1u << 30);
  for (auto e : dataset.stones ())
    bound = std::min (bound, e.weight);
  int full = std::min (dataset.num_cities (), dataset.num_stones ());
  while (bound < (1u << 30) && (int)edges.size () < full) {
    augment (bound);
    bound += (bound / 200) + 1;
  }
  return edges;
}
// ---
inline auto final_improve (StoneMatching& stones, SimpleRoute& route) -> double
{
  auto const& dataset = route.dataset ();
  auto score = evaluate (route, stones);
  auto total = 0.0;
  for (int i = 1; i < dataset.num_cities (); ++i) {
    for (int j = i + 1; j < i + 10 && j < dataset.num_cities (); ++j) {
      route.reverse (i, j);
      auto new_score = evaluate (route, stones);
      if (new_score.score > score.score) {
        total += new_score.score - score.score;
        score = new_score;
      } else {
        route.reverse (i, j);
      }
    }
    for (int j = dataset.num_cities () - 1; j > i && j > dataset.num_cities () - 10; --j) {
      route.reverse (i, j);
      auto new_score = evaluate (route, stones);
      if (new_score.score > score.score) {
        total += new_score.score - score.score;
        score = new_score;
      } else {
        route.reverse (i, j);
      }
    }
  }
  return total;
}
inline auto final_improve_random (StoneMatching const& stones, SimpleRoute& route, std::mt19937& rng, int rounds) -> double
{
  auto const& dataset = route.dataset ();
  auto score = evaluate (route, stones);
  double tot = 0;
  while (rounds-- > 0) {
    int idx1 = (rng () % (dataset.num_cities () - 1) + 1);
    int idx2 = (rng () % (dataset.num_cities () - 1) + 1);
    if (idx1 > idx2)
      std::swap (idx1, idx2);
    route.reverse (idx1, idx2);
    auto new_score = evaluate (route, stones);
    if (new_score.score > score.score) {
      tot += new_score.score - score.score;
      score = new_score;
    } else {
      route.reverse (idx1, idx2);
    }
  }
  return tot;
}
inline auto print_statistics (Matching const& matching, Dataset const& dataset) -> void
{
  long long energy = 0;
  int weight = 0;
  for (auto e : matching) {
    auto s = dataset.stone (e.first);
    energy += s.energy;
    weight += s.weight;
  }
  std::cerr << "energy: " << energy << "\n";
  std::cerr << "weight: " << weight << "\n";
}
inline auto solve_tsp_matching (Dataset const& dataset) -> std::pair<SimpleRoute, StoneMatching>
{
  // compute a tsp solution
  auto rng = std::mt19937 (std::random_device {}());
  auto route = tsp_solve (dataset, rng, std::chrono::milliseconds (1000));
  auto matching = StoneMatching (dataset);
  auto score = evaluate (route, matching);
  auto const matching_strategies = {make_matching_greedy1, //
    make_matching_mcbm,
    make_matching_mcbm_weight};
  auto const subset_strategies = {select_subset_heuristic_energy, //
    select_subset_heuristic_random,
    select_subset_heuristic_ratio,
    select_subset_heuristic_weight,
    select_subset_heuristic_heavy};
  for (auto matching_strategy : matching_strategies) {
    auto match = matching_strategy (dataset, route);
    for (auto subset_strategy : subset_strategies) {
      auto curr = subset_strategy (match, route);
      auto new_score = evaluate (route, curr);
      if (new_score.score > score.score) {
        score = new_score;
        matching = std::move (curr);
      }
    }
  }
  auto improve = final_improve_random (matching, route, rng, 1000);
  return {std::move (route), std::move (matching)};
}
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
int main ()
{
  std::ios_base::sync_with_stdio (false);
#ifdef EVAL
  auto is = std::ifstream ("input.txt");
  auto os = std::ofstream ("output.txt");
#else
  auto& is = std::cin;
  auto& os = std::cout;
#endif
  is.tie (0);
  os.tie (0);
  is.sync_with_stdio (false);
  os.sync_with_stdio (false);
  auto const data = read_dataset (is);
  auto const sol = solve_tsp_matching (data);
  write_output (os, sol.first, sol.second);
}
