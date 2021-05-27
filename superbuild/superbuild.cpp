#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
#include <algorithm>
#include <array>
#include <bitset>
#include <functional>
#include <vector>
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
// Limits
constexpr auto MAX_CITIES = 2000;
constexpr auto MAX_EDGES = (MAX_CITIES * (MAX_CITIES - 1)) / 2;
constexpr auto MAX_STONES = 10000;
constexpr auto MAX_GLOVE_CAPACITY = 10000000;
constexpr auto MAX_GLOVE_RESISTANCE = 5000.0;
constexpr auto MAX_STONE_WEIGHT = 100000;
constexpr auto MAX_STONE_ENERGY = 100000;
constexpr auto MAX_VELOCITY = 1000.0;
constexpr auto MAX_DISTANCE = 10000;
// CompleteGraph
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
    ASSERT (from != to);
    if (from < to)
      std::swap (from, to);
    return (from * (from - 1)) / 2 + to;
  }
  auto distance (int from, int to) const& -> int const&
  {
    ASSERT (from != to);
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return _distances[index (from, to)];
  }
  auto distance (int from, int to) & -> int&
  {
    ASSERT (from != to);
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return _distances[index (from, to)];
  }
  auto num_cities () const -> int
  {
    return _num_cities;
  }
};
// Stone
struct Stone
{
  int weight;
  int energy;
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
// StoneIndex
struct StoneIndex
{
private:
  std::vector<Stone> _stones;
  std::vector<std::vector<int>> _stones_per_city;
  std::vector<std::vector<int>> _cities_with_stone;
public:
  StoneIndex (int num_stones, int num_cities)
    : _stones (num_stones),          //
      _stones_per_city (num_cities), //
      _cities_with_stone (num_stones)
  {
    ASSERT (num_stones >= 0 && num_stones <= MAX_STONES);
    ASSERT (num_cities >= 0 && num_cities <= MAX_CITIES);
  }
  auto num_stones () const -> int
  {
    return _stones.size ();
  }
  auto num_cities () const -> int
  {
    return _stones_per_city.size ();
  }
  auto stones () const -> std::vector<Stone> const&
  {
    return _stones;
  }
  auto stones () -> std::vector<Stone>&
  {
    return _stones;
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
  auto store (int stone_id, int city_id)
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    ASSERT (city_id >= 0 && city_id <= num_cities ());
    _stones_per_city[city_id].push_back (stone_id);
    _cities_with_stone[stone_id].push_back (city_id);
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
    for (int i = 0; i < num_cities (); ++i) {
      std::sort (_stones_per_city[i].begin (), _cities_with_stone[i].end ());
    }
    for (int i = 0; i < num_stones (); ++i) {
      std::sort (_cities_with_stone[i].begin (), _cities_with_stone[i].end ());
    }
  }
  auto has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return std::binary_search (_stones_per_city[city_id].begin (), _stones_per_city[city_id].end (), stone_id);
  }
};
// Glove
struct Glove
{
  int capacity;
  double resistance;
  Glove (int capacity, double resistance) : capacity (capacity), resistance (resistance)
  {}
};
// Dataset
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
    ASSERT (_graph.num_cities () >= 1 && _graph.num_cities () <= MAX_CITIES);
    for (int i = 0; i < _graph.num_cities (); ++i)
      for (int j = i + 1; j < _graph.num_cities (); ++j)
        ASSERT (_graph.distance (i, j) >= 0 && _graph.distance (i, j) <= MAX_DISTANCE);
    ASSERT (_stones.num_stones () >= 0 && _stones.num_stones () <= MAX_STONES);
    ASSERT (_stones.num_cities () == _graph.num_cities ());
    for (Stone stone : _stones.stones ()) {
      ASSERT (stone.weight >= 0 && stone.weight <= MAX_STONE_WEIGHT);
      ASSERT (stone.energy >= 0 && stone.energy <= MAX_STONE_ENERGY);
    }
    ASSERT (_glove.capacity >= 0 && _glove.capacity <= MAX_GLOVE_CAPACITY);
    ASSERT (_glove.resistance >= 0 && _glove.resistance <= MAX_GLOVE_RESISTANCE);
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
    ASSERT (from != to);
    return _graph.distance (from, to);
  }
  auto travel_distance (int from, int to, int weight) const -> int
  {
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    ASSERT (from != to);
    ASSERT (weight >= 0 && weight <= glove_capacity ());
    auto const delta_velocity = max_velocity () - min_velocity ();
    auto const slowness_factor = delta_velocity / glove_capacity ();
    auto const current_velocity = std::max (min_velocity (), max_velocity () - weight * slowness_factor);
    return distance (from, to) / current_velocity;
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
};
// --- Route
struct Route
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  std::array<int, MAX_CITIES> _visit_index {};
  std::array<int, MAX_CITIES> _route {};
  int _size {};
  int _length {};
public:
  Route (Dataset const& dataset) : _dataset (dataset)
  {
    std::fill (_visit_index.begin (), _visit_index.end (), -1);
    auto const c = dataset.starting_city ();
    _visit_index[c] = 0;
    _route[_size++] = c;
  }
  auto dataset () const -> Dataset const&
  {
    return _dataset.get ();
  }
  auto is_cycle () const -> bool
  {
    return size () == dataset ().num_cities ();
  }
  auto size () const -> int
  {
    return _size;
  }
  auto front () const -> int
  {
    return _route.front ();
  }
  auto back () const -> int
  {
    return _route[size () - 1];
  }
  auto push_back (int city_id) -> void
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (!is_cycle ());
    ASSERT (!is_visited (city_id));
    _length += dataset ().distance (back (), city_id);
    _route[_size++] = city_id;
    _visit_index[city_id] = _size - 1;
  }
  auto pop_back () -> void
  {
    ASSERT (size () > 1);
    auto const erased = back ();
    _visit_index[back ()] = -1;
    _size--;
    _length -= dataset ().distance (back (), erased);
  }
  auto length () const -> int
  {
    return _length;
  }
  auto begin () const -> std::array<int, MAX_CITIES + 1>::const_iterator
  {
    return _route.begin ();
  }
  auto end () const -> std::array<int, MAX_CITIES + 1>::const_iterator
  {
    return begin () + size ();
  }
  auto operator[] (int pos) const -> int
  {
    ASSERT (pos >= 0 && pos < size ());
    return _route[pos];
  }
  auto is_visited (int city_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return _visit_index[city_id] != -1;
  }
  auto index (int city_id) const -> int
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (is_visited (city_id));
    return _visit_index[city_id];
  }
  auto flip (int from, int to) -> int
  {
    ASSERT (from > 0 && from < size ());
    ASSERT (to > 0 && to < size ());
    auto const l1 = _length;
    auto const c1 = _route[from];
    auto const c2 = _route[to];
    std::reverse (_route.begin () + from, _route.begin () + to + 1);
    auto const lhs = _route[(from - 1 + size ()) % size ()];
    auto const rhs = _route[(to + 1) % size ()];
    _length = _length - dataset ().distance (lhs, c1) //
      - dataset ().distance (c2, rhs)                 //
      + dataset ().distance (lhs, c2)                 //
      + dataset ().distance (c1, rhs);
    return length () - l1;
  }
  auto flip_profit (int from, int to) -> int
  {
    ASSERT (from > 0 && from < size ());
    ASSERT (to > 0 && to < size ());
    auto const c1 = _route[from];
    auto const c2 = _route[to];
    auto const lhs = _route[(from - 1 + size ()) % size ()];
    auto const rhs = _route[(to + 1) % size ()];
    return dataset ().distance (lhs, c2) + dataset ().distance (c1, rhs) //
      - dataset ().distance (lhs, c1) - dataset ().distance (c2, rhs);
  }
  template<class Fn>
  auto for_each_vertex (Fn fn) const -> void
  {
    for (int i = 0; i < size (); ++i) {
      fn (_route[i]);
    }
  }
  template<class Fn>
  auto for_each_edge (Fn fn) const -> void
  {
    for (int i = 0; i + 1 < size (); ++i) {
      fn (_route[i], _route[i + 1]);
    }
    if (is_cycle ())
      fn (back (), front ());
  }
};
// --- StoneMatching
struct StoneMatching
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  std::array<int, MAX_STONES> _matched_city;
  std::array<int, MAX_CITIES> _matched_stone;
  int _weight = 0;
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
  }
  auto unmatch_stone (int stone_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (is_stone_matched (stone_id));
    auto const city_id = matched_city (stone_id);
    _matched_city[stone_id] = -1;
    _matched_stone[city_id] = -1;
    _weight -= dataset ().stone (stone_id).weight;
  }
  auto unmatch_city (int city_id) -> void
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (is_city_matched (city_id));
    auto const stone_id = matched_stone (city_id);
    _matched_stone[city_id] = -1;
    _matched_city[stone_id] = -1;
    _weight -= dataset ().stone (stone_id).weight;
  }
  auto weight () const -> int
  {
    return _weight;
  }
};
// --- Evaluation
struct Evaluation
{
  int energy;
  double time_elapsed;
  double score;
};
inline auto evaluate (Route const& route, StoneMatching const& matching) -> Evaluation
{
  auto const& dataset = route.dataset ();
  ASSERT (route.is_cycle ());
  ASSERT (route.size () == dataset.num_cities ());
  int weight = 0;
  int energy = 0;
  double time_elapsed = 0.0;
  route.for_each_edge ([&] (int from, int to) {
    if (matching.is_city_matched (from)) {
      int stone_id = matching.matched_stone (from);
      weight += dataset.stone (stone_id).weight;
      energy += dataset.stone (stone_id).energy;
      ASSERT (weight <= dataset.glove_capacity ());
    }
    time_elapsed += dataset.travel_distance (from, to, weight);
  });
  return Evaluation {energy, time_elapsed, energy - time_elapsed * dataset.glove_resistance ()};
}
#include <random>
// Start from the first vertex, going forwards, take a random stone until
// the glove is at full capacity.
inline auto select_random_stones (Dataset const& dataset, //
  Route const& route) -> StoneMatching
{
  auto rng = std::mt19937 (std::random_device {}());
  auto matching = StoneMatching (dataset);
  auto best_score = evaluate (route, matching);
  auto weight = 0;
  route.for_each_edge ([&] (int from, int to) {
    auto available_stones = dataset.stones_at_city (from);
    // erase matched stones
    [&] () {
      auto const is_matched = [&matching] (int stone_id) { return matching.is_stone_matched (stone_id); };
      auto last = std::remove_if (available_stones.begin (), available_stones.end (), is_matched);
      available_stones.erase (last, available_stones.end ());
    }();
    // take a prefix of at most 15 stones
    if (available_stones.size () > 15)
      available_stones.resize (15);
    // shuffle
    std::shuffle (available_stones.begin (), available_stones.end (), rng);
    // take the best stone
    int best_stone = -1;
    for (int stone_id : available_stones) {
      if (!matching.is_stone_matched (stone_id)) {
        if (weight + dataset.stone (stone_id).weight <= dataset.glove_capacity ()) {
          // try to match and compute a new score
          matching.match (stone_id, from);
          auto const new_score = evaluate (route, matching);
          if (new_score.score > best_score.score) {
            best_score = new_score;
            best_stone = stone_id;
          }
          // unmatch to try new stones
          matching.unmatch_stone (stone_id);
        }
      }
    }
    if (best_stone != -1) {
      matching.match (best_stone, from);
      weight += dataset.stone (best_stone).weight;
    }
  });
  return matching;
}
inline auto select_random_route (Dataset const& dataset) -> Route
{
  auto rng = std::mt19937 (std::random_device {}());
  auto result = Route (dataset);
  auto order = std::array<int, MAX_CITIES + 1> {};
  std::iota (order.begin (), order.begin () + dataset.num_cities (), 0);
  std::shuffle (order.begin (), order.begin () + dataset.num_cities (), rng);
  for (int i = 0; i < dataset.num_cities (); ++i)
    if (order[i] != dataset.starting_city ())
      result.push_back (order[i]);
  return result;
}
inline auto select_greedy_route (Dataset const& dataset) -> Route
{
  auto result = Route (dataset);
  while (result.size () != dataset.num_cities ()) {
    int curr_best = -1;
    int best_dist = 1u << 30;
    for (int i = 0; i < dataset.num_cities (); ++i) {
      if (!result.is_visited (i)) {
        if (dataset.distance (i, result.back ()) < best_dist) {
          best_dist = dataset.distance (i, result.back ());
          curr_best = i;
        }
      }
    }
    result.push_back (curr_best);
  }
  return result;
}
inline auto baseline (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_random_route (dataset);
  auto const stones = select_random_stones (dataset, route);
  return {std::move (route), std::move (stones)};
}
inline auto baseline_greedy (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_greedy_route (dataset);
  auto const stones = select_random_stones (dataset, route);
  return {std::move (route), std::move (stones)};
}
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
  ASSERT (route.size () == dataset.num_cities ());
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
#pragma
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
#pragma
#include <random>
inline auto select_tour_tsp (Dataset const& dataset) -> Route
{
  auto const bootstrap = [&dataset] () -> Route {
    auto route = Route (dataset);
    auto rng = std::mt19937 (std::random_device {}());
    auto indices = std::vector<int> (dataset.num_cities ());
    std::iota (indices.begin (), indices.end (), 0);
    std::shuffle (indices.begin (), indices.end (), rng);
    while (route.size () != dataset.num_cities ()) {
      int curr_best = -1;
      int curr_dist = 1u << 30;
      for (int i : indices) {
        if (!route.is_visited (i) && dataset.distance (i, route.back ()) < curr_dist) {
          curr_best = i;
          curr_dist = dataset.distance (i, route.back ());
        }
      }
      route.push_back (curr_best);
    }
    return route;
  };
  auto route = bootstrap ();
  int threshold = 1024;
  while (threshold > 0) {
    bool improved = false;
    for (int i = 1; i < route.size (); ++i) {
      for (int j = i + 2; j < route.size (); ++j) {
        if (route.flip_profit (i, j) <= -threshold) {
          route.flip (i, j);
          improved = true;
        }
      }
    }
    if (!improved)
      threshold /= 2;
  }
  return route;
}
inline auto tsp_matching_opt (Dataset const& dataset) -> std::pair<Route, StoneMatching>
{
  auto const route = select_tour_tsp (dataset);
  auto const matching = select_stones_matching (dataset, [&] (int stone_id, int city_id) -> double {
    auto const idx = route.index (city_id);
    return dataset.stone (stone_id).weight * (dataset.num_cities () - idx);
  });
  return {route, matching};
}
int main ()
{
#ifdef EVAL
  auto is = std::ifstream ("input.txt");
  auto os = std::ofstream ("output.txt");
#else
  auto& is = std::cin;
  auto& os = std::cout;
#endif
  auto rng = std::mt19937 (std::random_device {}());
  auto const data = read_dataset (is);
  auto const sol = tsp_matching_opt (data);
  write_output (os, sol.first, sol.second);
}
