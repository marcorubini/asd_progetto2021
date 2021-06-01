#include <algorithm>
#include <chrono>
#include <fstream>
#include <numeric>
#include <random>
constexpr auto MAX_CITIES = 2000;
constexpr auto MAX_EDGES = (MAX_CITIES * (MAX_CITIES + 1)) / 2;
constexpr auto MAX_STONES = 10000;
constexpr auto MAX_GLOVE_CAPACITY = 10000000;
constexpr auto MAX_GLOVE_RESISTANCE = 5000.0;
constexpr auto MAX_STONE_WEIGHT = 100000;
constexpr auto MAX_STONE_ENERGY = 100000;
constexpr auto MAX_VELOCITY = 1000.0;
constexpr auto MAX_DISTANCE = 10000;
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#if defined(EVAL) || defined(RELEASE)
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
#if defined(EVAL) || defined(RELEASE)
#  define CHECK(...)                                                                                         \
    do {                                                                                                     \
      if (!(__VA_ARGS__))                                                                                    \
        __builtin_unreachable ();                                                                            \
    } while (0)
#else
#  define CHECK(...)                                                                                         \
    do {                                                                                                     \
      if (!(__VA_ARGS__)) {                                                                                  \
        fprintf (stderr, "CHECK FAILED: %s\n", #__VA_ARGS__);                                                \
        fprintf (stderr, "LINE: %d FILE: %s FUNCTION: %s\n", __LINE__, __FILE__, __FUNCTION__);              \
        std::terminate ();                                                                                   \
      }                                                                                                      \
    } while (0)
#endif
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Milli = std::chrono::milliseconds;
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
  using distance_type = std::int16_t;
private:
  std::vector<distance_type> _distances = std::vector<distance_type> (MAX_EDGES);
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
  auto distance (int from, int to) const& -> distance_type const&
  {
    ASSERT (from >= 0 && from < num_cities ());
    ASSERT (to >= 0 && to < num_cities ());
    return _distances[index (from, to)];
  }
  auto distance (int from, int to) & -> distance_type&
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
#include <algorithm>
#include <array>
#include <bitset>
#include <deque>
#include <vector>
// Stores information about all stones and in which cities they can be found.
struct StoneIndex
{
  using index_type = std::int16_t;
private:
  std::array<Stone, MAX_STONES> _stones {};
  std::vector<std::vector<index_type>> _cities_with_stone {};
  std::vector<std::bitset<MAX_STONES>> _city_has_stone {};
  int _num_stones {};
  int _num_cities {};
public:
  StoneIndex (int num_stones, int num_cities) //
    : _cities_with_stone (num_stones),        //
      _num_stones (num_stones),               //
      _num_cities (num_cities),               //
      _city_has_stone (num_cities)
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
    _cities_with_stone[stone_id].push_back (city_id);
    _city_has_stone[city_id].set (stone_id);
  }
  auto cities_with_stone (int stone_id) const -> std::vector<index_type> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _cities_with_stone[stone_id];
  }
  auto city_has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _city_has_stone[city_id].test (stone_id);
  }
};
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
  auto unmatch (int stone_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (is_stone_matched (stone_id));
    auto const city_id = matched_city (stone_id);
    _matched_city[stone_id] = -1;
    _matched_stone[city_id] = -1;
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
  auto fits (int weight) const -> bool
  {
    return weight + this->weight () <= dataset ().glove_capacity ();
  }
};
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
namespace Tsp
{
  template<class DistFn>
  inline auto tsp_bootstrap_greedy (int* const first, int n, DistFn dist_fn, std::mt19937& rng) -> int
  {
    auto indices = std::vector<int> (n);
    std::iota (indices.begin (), indices.end (), 0);
    std::shuffle (indices.begin (), indices.end (), rng);
    auto cost = 0;
    for (int i = 0; i + 1 < n; ++i) {
      auto best_cost = std::numeric_limits<int>::max ();
      auto best_it = i + 1;
      for (int j : indices) {
        if (j > i && dist_fn (first[i], first[j]) < best_cost) {
          best_cost = dist_fn (first[i], first[j]);
          best_it = j;
        }
      }
      std::swap (first[i + 1], first[best_it]);
      cost += dist_fn (first[i], first[i + 1]);
    }
    cost += dist_fn (first[0], first[n - 1]);
    return cost;
  }
  template<class DistFn>
  inline auto tsp_bootstrap_greedy (int* const first, int n, DistFn dist_fn, int rounds, std::mt19937& rng) -> int
  {
    auto result = std::vector<int> (first, first + n);
    auto best = tsp_bootstrap_greedy (result.data (), n, dist_fn, rng);
    while (rounds-- > 0) {
      auto curr = tsp_bootstrap_greedy (first, n, dist_fn, rng);
      if (curr < best) {
        best = curr;
        std::copy (first, first + n, result.begin ());
      }
    }
    std::copy (result.begin (), result.end (), first);
    return best;
  }
  template<class DistFn>
  inline auto tsp_opt2 (int* first, int n, int x, int y, DistFn dist_fn) -> int
  {
    if (x == y)
      return 0;
    if (x > y)
      std::swap (x, y);
    if (x == 0 && y == (n - 1))
      return 0;
    auto const c1 = (x == 0) ? (n - 1) : (x - 1);
    auto const c2 = x;
    auto const c3 = y;
    auto const c4 = (y + 1 == n) ? (0) : (y + 1);
    auto const l1 = dist_fn (first[c1], first[c2]);
    auto const l2 = dist_fn (first[c3], first[c4]);
    auto const L1 = dist_fn (first[c1], first[c3]);
    auto const L2 = dist_fn (first[c2], first[c4]);
    if (L1 + L2 < l1 + l2) {
      std::reverse (first + x, first + y + 1);
      return L1 + L2 - l1 - l2;
    } else {
      return 0;
    }
  }
  template<class DistFn>
  inline auto tsp_opt3 (int* first, int n, int a, int b, int c, DistFn dist_fn) -> int
  {
    if (a == b || a == c || b == c)
      return 0;
    int a1 = first[a ? a - 1 : n - 1];
    int a2 = first[a];
    int b1 = first[b - 1];
    int b2 = first[b];
    int c1 = first[c - 1];
    int c2 = first[c == n ? 0 : c];
    auto d0 = dist_fn (a1, a2) + dist_fn (b1, b2) + dist_fn (c1, c2);
    auto d1 = dist_fn (a1, b1) + dist_fn (a2, b2) + dist_fn (c1, c2);
    auto d2 = dist_fn (a1, a2) + dist_fn (b1, c1) + dist_fn (b2, c2);
    auto d3 = dist_fn (a1, b2) + dist_fn (c1, a2) + dist_fn (b1, c2);
    auto d4 = dist_fn (c2, a2) + dist_fn (b1, b2) + dist_fn (c1, a1);
    if (d0 > d1) {
      std::reverse (first + a, first + b);
      return -d0 + d1;
    }
    if (d0 > d2) {
      std::reverse (first + b, first + c);
      return -d0 + d2;
    }
    if (d0 > d4) {
      std::reverse (first + a, first + c);
      return -d0 + d4;
    }
    if (d0 > d3) {
      auto tmp = std::vector<int> ((c - b) + (b - a));
      std::copy (first + b, first + c, tmp.data ());
      std::copy (first + a, first + b, tmp.data () + (c - b));
      std::copy (tmp.begin (), tmp.end (), first + a);
      return -d0 + d3;
    }
    return 0;
  }
  template<class DistFn>
  inline auto tsp_improve_random3 (int* first, int n, DistFn dist_fn, std::mt19937& rng) -> int
  {
    auto improved = 0;
    for (int i = 0; i < 10; ++i) {
      int x = rng () % n;
      int y = rng () % n;
      int z = rng () % n;
      if (y < x)
        std::swap (x, y);
      if (z < x)
        std::swap (x, z);
      if (z < y)
        std::swap (y, z);
      improved += tsp_opt3 (first, n, x, y, z, dist_fn);
    }
    return improved;
  }
  template<class DistFn>
  inline auto tsp_improve_random2 (int* first, int n, DistFn dist_fn, std::mt19937& rng) -> int
  {
    auto improved = 0;
    for (int i = 0; i < 10; ++i) {
      int x = rng () % n;
      int y = rng () % n;
      if (y < x)
        std::swap (x, y);
      improved += tsp_opt2 (first, n, x, y, dist_fn);
    }
    return improved;
  }
  template<class DistFn>
  inline auto tsp (int* first, int n, int start, DistFn dist_fn, std::mt19937& rng, double allowed_ms = 1000) -> int
  {
    auto const time_start = std::clock ();
    auto const time_elapsed = [time_start] () -> double {
      auto now = std::clock ();
      return std::difftime (now, time_start) * 1000.0 / CLOCKS_PER_SEC;
    };
    auto first_cost = tsp_bootstrap_greedy (first, n, dist_fn, 8, rng);
    int cost = 0;
    while (time_elapsed () < allowed_ms * 0.3)
      cost += tsp_improve_random3 (first, n, dist_fn, rng);
    if (cost < -50) {
      while (time_elapsed () < allowed_ms * 0.90)
        cost += tsp_improve_random3 (first, n, dist_fn, rng);
      while (time_elapsed () < allowed_ms)
        cost += tsp_improve_random2 (first, n, dist_fn, rng);
    }
    std::rotate (first, std::find (first, first + n, start), first + n);
    return cost + first_cost;
  }
} // namespace Tsp
#include <algorithm>
#include <array>
#include <functional>
struct SimpleRoute
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  std::array<int, MAX_CITIES> _route {};
  std::array<int, MAX_CITIES> _visited_index {};
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
      ++size;
    }
  }
  SimpleRoute (Dataset const& dataset, int const* first, int const* last) //
    : _dataset (dataset)
  {
    ASSERT (last - first == dataset.num_cities ());
    ASSERT (*first == dataset.starting_city ());
    std::copy (first, last, _route.begin ());
    for (int i = 0; i < dataset.num_cities (); ++i)
      _visited_index[_route[i]] = i;
  }
  auto dataset () const -> Dataset const&
  {
    return _dataset.get ();
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
  auto reverse (int idx1, int idx2) -> void
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());
    if (idx1 == idx2)
      return;
    auto const n = dataset ().num_cities ();
    std::reverse (_route.begin () + idx1, _route.begin () + idx2 + 1);
    for (int i = idx1; i <= idx2; ++i)
      _visited_index[_route[i]] = i;
  }
  auto swap (int idx1, int idx2) -> void
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());
    if (idx1 == idx2)
      return;
    auto const n = dataset ().num_cities ();
    std::swap (_route[idx1], _route[idx2]);
    std::swap (_visited_index[at (idx1)], _visited_index[at (idx2)]);
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
#include <vector>
namespace Flow
{
  struct VisitSet
  {
    std::vector<int> v;
    int v_id;
    VisitSet (int N) : v (N), v_id (1)
    {}
    auto assign (int N) -> void
    {
      v.assign (N, v_id);
    }
    auto reset () -> void
    {
      v_id++;
    }
    auto visit (int x) -> void
    {
      ASSERT (x >= 0 && x < (int)v.size ());
      v[x] = v_id;
    }
    auto unvisit (int x) -> void
    {
      ASSERT (x >= 0 && x < (int)v.size ());
      v[x] = -1;
    }
    auto visited (int x) const -> bool
    {
      ASSERT (x >= 0 && x < (int)v.size ());
      return v[x] == v_id;
    }
  };
  struct DoubleBuffer
  {
    std::vector<int> buf[2] {};
    auto push (int x) -> void
    {
      buf[1].push_back (x);
    }
    auto back () -> int
    {
      if (buf[0].size () == 0)
        swap (buf[0], buf[1]);
      return buf[0].back ();
    }
    auto pop () -> void
    {
      buf[0].pop_back ();
    }
    auto empty () const -> bool
    {
      return buf[0].size () == 0 && buf[1].size () == 0;
    }
    auto clear () -> void
    {
      buf[0].clear ();
      buf[1].clear ();
    }
  };
  struct WeightedDinic
  {
    struct Edge
    {
      int from;
      int to;
      int flow;
      int capacity;
      int cost;
      Edge () = default;
      Edge (int from, int to, int flow, int capacity, int cost) //
        : from (from), to (to), flow (flow), capacity (capacity), cost (cost)
      {}
      auto residual () const -> int
      {
        return capacity - flow;
      }
      auto saturated () const -> bool
      {
        return residual () <= 0;
      }
    };
    struct Node
    {
      std::vector<int> outgoing_edges {};
      int level {};
      long long distance {};
      std::vector<int>::iterator next_edge {};
    };
    std::vector<Edge> edges;
    std::vector<Node> nodes;
    VisitSet visited;
    DoubleBuffer queue;
    WeightedDinic (int N) //
      : edges (),         //
        nodes (N),        //
        visited (N),      //
        queue ()
    {}
    auto add (int from, int to, int capacity, int cost) -> void
    {
      edges.push_back ({from, to, 0, capacity, cost});
      edges.push_back ({to, from, 0, 0, -cost});
      nodes[from].outgoing_edges.push_back (edges.size () - 2);
      nodes[to].outgoing_edges.push_back (edges.size () - 1);
    }
    auto spfa (int src, int sink) -> bool
    {
      visited.reset ();
      queue.clear ();
      for (auto& node : nodes)
        node.distance = 1ull << 62;
      visited.visit (src);
      nodes[src].level = 0;
      nodes[src].distance = 0;
      queue.push (src);
      while (!queue.empty ()) {
        auto const curr = queue.back ();
        queue.pop ();
        visited.unvisit (curr);
        for (int edge_id : nodes[curr].outgoing_edges) {
          auto const edge = edges[edge_id];
          if (!edge.saturated () && nodes[edge.to].distance > nodes[curr].distance + edge.cost) {
            nodes[edge.to].level = nodes[curr].level + 1;
            nodes[edge.to].distance = nodes[curr].distance + edge.cost;
            if (visited.visited (edge.to))
              continue;
            visited.visit (edge.to);
            queue.push (edge.to);
          }
        }
      }
      return nodes[sink].distance != (1ull << 62);
    }
    auto augment (int edge_id, int bottleneck) -> void
    {
      edges[edge_id].flow += bottleneck;
      edges[edge_id ^ 1].flow -= bottleneck;
    }
    auto dfs (int curr, int sink, int bottleneck, long long dist) -> int
    {
      if (curr == sink)
        return bottleneck;
      while (nodes[curr].next_edge != nodes[curr].outgoing_edges.end ()) {
        auto const edge_id = *nodes[curr].next_edge;
        auto const edge = edges[edge_id];
        if (!edge.saturated () && nodes[edge.to].level == nodes[curr].level + 1) {
          if (dist + edge.cost <= nodes[edge.to].distance) {
            auto const pushed = dfs (edge.to, sink, std::min (bottleneck, edge.residual ()), dist + edge.cost);
            if (pushed > 0) {
              augment (edge_id, pushed);
              return pushed;
            }
          }
        }
        nodes[curr].next_edge++;
      }
      return 0;
    }
    auto solve (int src, int sink) -> std::pair<int, long long>
    {
      auto flow = 0;
      auto cost = 0ll;
      while (spfa (src, sink)) {
        for (auto& node : nodes)
          node.next_edge = node.outgoing_edges.begin ();
        auto pushed = dfs (src, sink, 1u << 30, 0ll);
        while (pushed > 0) {
          flow += pushed;
          cost += 1ll * pushed * nodes[sink].distance;
          pushed = dfs (src, sink, 1u << 30, 0ll);
        }
      }
      return {flow, cost};
    }
    template<class Fn>
    auto for_each_flow_edge (Fn fn) const -> void
    {
      for (auto e : edges)
        if (e.flow > 0)
          fn (e);
    }
  };
} // namespace Flow
#include <algorithm>
namespace BipartiteMatching
{
  struct WeightedMaxCardinalityBipartiteMatching
  {
    Flow::WeightedDinic dinic;
    int size1;
    auto source () const -> int
    {
      return 0;
    }
    auto sink () const -> int
    {
      return 1;
    }
    auto left (int i) const -> int
    {
      return i + 2;
    }
    auto right (int i) const -> int
    {
      return size1 + i + 2;
    }
    auto rev_left (int i) const -> int
    {
      return i - 2;
    }
    auto rev_right (int i) const -> int
    {
      return i - 2 - size1;
    }
    WeightedMaxCardinalityBipartiteMatching (int size1, int size2) //
      : dinic (size1 + size2 + 2),                                 //
        size1 (size1)
    {
      for (int i = 0; i < size1; ++i)
        dinic.add (source (), left (i), 1, 0);
      for (int i = 0; i < size2; ++i)
        dinic.add (right (i), sink (), 1, 0);
    }
    auto add (int from, int to, int cost) -> void
    {
      dinic.add (left (from), right (to), 1, cost);
    }
    auto solve () -> std::pair<int, long long>
    {
      return dinic.solve (source (), sink ());
    }
    template<class Fn>
    auto for_each_match (Fn fn) const -> void
    {
      dinic.for_each_flow_edge ([=] (Flow::WeightedDinic::Edge edge) {
        if (edge.from != source () && edge.to != sink ())
          fn (rev_left (edge.from), rev_right (edge.to));
      });
    }
  };
  // Space optimized Hopcroft Karp algorithm inspired by https://codeforces.com/blog/entry/58048
  // https://pastebin.com/q12aBwya
  struct HopcroftKarp
  {
    int size1;
    int size2;
    std::vector<std::vector<int>> adjacency;
    std::vector<int> L;
    std::vector<int> R;
    std::vector<bool> visited;
    HopcroftKarp (int size1, int size2)
      : size1 (size1), size2 (size2), //
        adjacency (size1), L (size1, -1), R (size2, -1), visited (size1, false)
    {}
    auto add (int from, int to) -> void
    {
      adjacency[from].push_back (to);
    }
    auto dfs (int curr) -> bool
    {
      if (visited[curr])
        return false;
      visited[curr] = true;
      for (auto other : adjacency[curr]) {
        if (R[other] == -1) {
          L[curr] = other;
          R[other] = curr;
          return true;
        }
      }
      for (auto other : adjacency[curr]) {
        if (dfs (R[other])) {
          L[curr] = other;
          R[other] = curr;
          return true;
        }
      }
      return false;
    }
    auto solve () -> int
    {
      bool done = false;
      while (!done) {
        done = true;
        std::fill (visited.begin (), visited.end (), false);
        for (int i = 0; i < size1; ++i)
          if (L[i] == -1)
            done &= !dfs (i);
      }
      return size1 - std::count (L.begin (), L.end (), -1);
    }
    template<class Fn>
    auto for_each_edge (Fn fn) const -> void
    {
      for (int i = 0; i < size1; ++i)
        if (L[i] != -1)
          fn (i, L[i]);
    }
  };
} // namespace BipartiteMatching
#include <algorithm>
#include <numeric>
#include <vector>
namespace Knapsack
{
  struct KnapsackSolution
  {
    std::vector<int> selection;
    int weight;
    long long value;
    bool exact = true;
    KnapsackSolution (std::vector<int> selection, int weight, long long value, bool exact)
      : selection (std::move (selection)), weight (weight), value (value), exact (exact)
    {}
  };
  template<class WeightFn, class ValueFn, class It>
  inline auto knapsack_forward_dp (int capacity, It first, It last, WeightFn weight_fn, ValueFn value_fn, long long* out) -> void
  {
    ASSERT (capacity >= 0);
    std::fill (out, out + capacity + 1, 0ll);
    if (capacity == 0)
      return;
    for (auto it = first; it != last; ++it) {
      auto const weight = weight_fn (*it);
      auto const value = 0ll + value_fn (*it);
      for (int w = capacity; w >= weight; w--)
        if (out[w] < out[w - weight] + value)
          out[w] = out[w - weight] + value;
    }
  }
  template<class WeightFn, class ValueFn, class It>
  inline auto knapsack_backward_dp (int capacity, It first, It last, WeightFn weight_fn, ValueFn value_fn, long long* out) -> void
  {
    using rit = std::reverse_iterator<It>;
    auto reverse_first = rit (last);
    auto reverse_last = rit (first);
    knapsack_forward_dp (capacity, reverse_first, reverse_last, weight_fn, value_fn, out);
  }
  template<class WeightFn, class ValueFn, class It>
  inline auto knapsack_hirschberg (int capacity, It first, It last, WeightFn weight_fn, ValueFn value_fn) -> KnapsackSolution
  {
    // can't take anything
    if (last == first || capacity == 0)
      return KnapsackSolution {std::vector<int> (), 0, 0ll, true};
    // only one choice
    if (last - first == 1) {
      if (weight_fn (*first) <= capacity && value_fn (*first) > 0) {
        return {std::vector<int> ({*first}), weight_fn (*first), 0ll + value_fn (*first), true};
      } else {
        return {std::vector<int> (), 0, 0ll, true};
      }
    }
    auto const mid = first + (last - first) / 2;
    auto dp1 = std::vector<long long> (capacity + 1);
    auto dp2 = std::vector<long long> (capacity + 1);
    knapsack_forward_dp (capacity, first, mid, weight_fn, value_fn, dp1.data ());
    knapsack_backward_dp (capacity, mid, last, weight_fn, value_fn, dp2.data ());
    auto best_value = std::numeric_limits<long long>::min ();
    auto best_weight = -1;
    for (int w = 0; w <= capacity; ++w) {
      if (dp1[w] + dp2[capacity - w] > best_value) {
        best_value = dp1[w] + dp2[capacity - w];
        best_weight = w;
      }
    }
    dp1 = {};
    dp2 = {};
    KnapsackSolution lhs = knapsack_hirschberg (best_weight, first, mid, weight_fn, value_fn);
    KnapsackSolution rhs = knapsack_hirschberg (capacity - best_weight, mid, last, weight_fn, value_fn);
    lhs.value += rhs.value;
    lhs.weight += rhs.weight;
    lhs.selection.insert (lhs.selection.end (), rhs.selection.begin (), rhs.selection.end ());
    return lhs;
  }
  template<class WeightFn, class ValueFn, class It>
  inline auto knapsack (int capacity, It first, It last, WeightFn weight_fn, ValueFn value_fn) -> KnapsackSolution
  {
    // can't take anything
    if (first == last || capacity == 0) {
      return KnapsackSolution {std::vector<int> (), 0, 0ll, true};
    }
    auto const total_weight = [&] () {
      auto res = 0ll;
      for (auto it = first; it != last; ++it)
        res += weight_fn (*it);
      return res;
    }();
    // take everything
    if (total_weight <= capacity) {
      auto result = std::vector<int> ();
      auto result_weight = 0;
      auto result_value = 0ll;
      for (auto it = first; it != last; ++it)
        result.push_back (*it), result_weight += weight_fn (*it), result_value += value_fn (*it);
      return KnapsackSolution {std::move (result), result_weight, result_value, true};
    }
    auto const constant_weight = [&] () {
      auto const first_weight = weight_fn (*first);
      for (auto it = first; it != last; ++it) {
        if (weight_fn (*it) != first_weight)
          return false;
      }
      return true;
    }();
    // sort by value
    if (constant_weight) {
      auto indices = std::vector<int> (last - first);
      std::iota (indices.begin (), indices.end (), 0);
      std::sort (indices.begin (), indices.end (), [&] (int a, int b) {
        return value_fn (first[a]) > value_fn (first[b]);
      });
      auto result = std::vector<int> ();
      auto curr_weight = 0;
      auto curr_value = 0ll;
      for (auto i : indices)
        if (curr_weight + weight_fn (first[i]) <= capacity)
          result.push_back (first[i]), curr_weight += weight_fn (first[i]), curr_value += value_fn (first[i]);
      return KnapsackSolution {std::move (result), curr_weight, curr_value, true};
    }
    auto const constant_value = [&] () {
      auto const first_value = value_fn (first[0]);
      for (auto it = first; it != last; ++it)
        if (value_fn (*it) != first_value)
          return false;
      return true;
    }();
    // sort by weight
    if (constant_value) {
      auto indices = std::vector<int> (last - first);
      std::iota (indices.begin (), indices.end (), 0);
      std::sort (indices.begin (), indices.end (), [&] (int a, int b) {
        return weight_fn (first[a]) < weight_fn (first[b]);
      });
      auto result = std::vector<int> ();
      auto curr_weight = 0;
      auto curr_value = 0ll;
      for (auto i : indices)
        if (curr_weight + weight_fn (first[i]) <= capacity)
          result.push_back (first[i]), curr_weight += weight_fn (first[i]), curr_value += value_fn (first[i]);
      return KnapsackSolution {std::move (result), curr_weight, curr_value, true};
    }
    // crash if too much space
    ASSERT (capacity <= 1000000000);
    // crash if too much time
    ASSERT (1ll * capacity * (last - first) <= 2000000000);
    // general solution
    return knapsack_hirschberg (capacity, first, last, weight_fn, value_fn);
  }
} // namespace Knapsack
#include <ctime>
struct Timer
{
private:
  std::clock_t time_start = std::clock ();
public:
  Timer () = default;
  auto elapsed_ms () const -> double
  {
    return std::difftime (std::clock (), time_start) * 1000.0 / CLOCKS_PER_SEC;
  }
};
#include <cmath>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
inline auto select_knapsack (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto const weight_fn = [&] (int id) -> int { return dataset.stone (id).weight; };
  auto const value_fn = [&] (int id) -> long long { return dataset.stone (id).energy; };
  return Knapsack::knapsack (dataset.glove_capacity (), indices.begin (), indices.end (), weight_fn, value_fn).selection;
}
inline auto find_matching (Dataset const& dataset, std::vector<int> selection) -> std::vector<std::pair<int, int>>
{
  std::sort (selection.begin (), selection.end (), [&] (int a, int b) {
    return dataset.stone (a).energy > dataset.stone (b).energy;
  });
  if (selection.size () > dataset.num_cities ())
    selection.resize (dataset.num_cities ());
  auto bip = BipartiteMatching::HopcroftKarp (selection.size (), dataset.num_cities ());
  for (int i = 0; i < (int)selection.size (); ++i)
    for (auto j : dataset.cities_with_stone (selection[i]))
      bip.add (i, j);
  auto matched = bip.solve ();
  auto result = std::vector<std::pair<int, int>> ();
  ASSERT (matched == selection.size ());
  bip.for_each_edge ([&] (int from, int to) { result.emplace_back (selection[from], to); });
  return result;
}
inline auto find_matching_heavy (SimpleRoute const& tour, std::vector<int> selection) -> std::vector<std::pair<int, int>>
{
  auto const& dataset = tour.dataset ();
  std::sort (selection.begin (), selection.end (), [&] (int a, int b) {
    return dataset.stone (a).energy > dataset.stone (b).energy;
  });
  if (selection.size () > dataset.num_cities ())
    selection.resize (dataset.num_cities ());
  auto bip = BipartiteMatching::WeightedMaxCardinalityBipartiteMatching (dataset.num_stones (), dataset.num_cities ());
  for (auto i : selection)
    for (auto j : dataset.cities_with_stone (i)) {
      auto w = dataset.stone (i).weight;
      auto c = tour.city_index (j);
      auto k = dataset.num_cities () - c + 1;
      bip.add (i, j, std::min (1ll * k * w, 10000000ll));
    }
  auto matched = bip.solve ().first;
  auto result = std::vector<std::pair<int, int>> ();
  ASSERT (matched == selection.size ());
  bip.for_each_match ([&] (int from, int to) { result.emplace_back (from, to); });
  return result;
}
inline auto solve_general (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const timer = Timer ();
  auto tour = [&] () {
    auto indices = std::vector<int> (dataset.num_cities ());
    std::iota (indices.begin (), indices.end (), 0);
    auto const length = Tsp::tsp (
      indices.data (),
      indices.size (),
      dataset.starting_city (),
      [&] (int x, int y) { return dataset.distance (x, y); },
      rng,
      allowed_ms * 0.45);
    return SimpleRoute (dataset, indices.data (), indices.data () + indices.size ());
  }();
  auto matching = StoneMatching (dataset);
  auto best_score = Evaluation {-1e9, 0, 0};
  for (auto select_strategy : {select_knapsack}) {
    auto found = std::vector<std::pair<int, int>> ();
    auto selected = select_strategy (dataset);
    if (selected.size () <= 170) {
      found = find_matching_heavy (tour, std::move (selected));
    } else {
      found = find_matching (dataset, select_strategy (dataset));
    }
    auto curr = StoneMatching (dataset);
    for (auto e : found)
      curr.match (e.first, e.second);
    auto new_score = evaluate (tour, curr);
    if (new_score.score > best_score.score) {
      matching = curr;
      best_score = new_score;
    }
  }
  auto stones = std::vector<int> ();
  for (int i = 0; i < dataset.num_stones (); ++i)
    if (matching.is_stone_matched (i))
      stones.push_back (i);
  std::sort (stones.begin (), stones.end (), [&] (int x, int y) {
    return dataset.stone (x).weight > dataset.stone (y).weight;
  });
  auto const improve_stone_pair = [&] (int x, int y) {
    auto c1 = matching.matched_city (x);
    auto c2 = matching.matched_city (y);
    if (tour.city_index (c1) > tour.city_index (c2)) {
      std::swap (x, y);
      std::swap (c1, c2);
    }
    if (dataset.stone (x).weight < dataset.stone (y).weight)
      return;
    if (dataset.city_has_stone (c1, y) && dataset.city_has_stone (c2, x)) {
      matching.unmatch (x);
      matching.unmatch (y);
      matching.match (x, c2);
      matching.match (y, c1);
    }
  };
  auto const improve_stone_pos = [&] (int x) {
    for (auto c2 : dataset.cities_with_stone (x)) {
      auto c1 = matching.matched_city (x);
      if (!matching.is_city_matched (c2) && tour.city_index (c2) > tour.city_index (c1)) {
        matching.unmatch (x);
        matching.match (x, c2);
      }
    }
  };
  auto const improve_reverse = [&] (int left, int right) {
    tour.reverse (left, right);
    auto new_score = evaluate (tour, matching);
    if (new_score.score > best_score.score) {
      best_score = new_score;
    } else {
      tour.reverse (left, right);
    }
  };
  auto const improve_swap = [&] (int left, int right) {
    tour.swap (left, right);
    auto new_score = evaluate (tour, matching);
    if (new_score.score > best_score.score) {
      best_score = new_score;
    } else {
      tour.swap (left, right);
    }
  };
  best_score = evaluate (tour, matching);
  auto initial_score = best_score;
  auto indices = std::vector<int> ();
  int iters = 0;
  auto const improve_round = [&] () {
    ++iters;
    for (int i = 0; i < 20; ++i) {
      int x = rng () % stones.size ();
      int y = rng () % stones.size ();
      if (x != y) {
        improve_stone_pair (stones[x], stones[y]);
      }
    }
    for (int i = 0; i < 20; ++i) {
      int x = rng () % (dataset.num_cities () - 1) + 1;
      int y = rng () % (dataset.num_cities () - 1) + 1;
      if (y < x)
        std::swap (x, y);
      if (x != y) {
        improve_reverse (x, y);
      }
    }
  };
  while (timer.elapsed_ms () < allowed_ms * 0.95)
    improve_round ();
  best_score = evaluate (tour, matching);
  for (auto s : stones) {
    auto c = matching.matched_city (s);
    matching.unmatch (s);
    auto new_score = evaluate (tour, matching);
    if (new_score.score > best_score.score) {
      best_score = new_score;
    } else {
      matching.match (s, c);
    }
  }
  for (auto s : stones)
    if (matching.is_stone_matched (s))
      improve_stone_pos (s);
  for (int i = 0; i < dataset.num_stones (); ++i) {
    if (!matching.is_stone_matched (i)) {
      if (matching.fits (dataset.stone (i).weight)) {
        auto k = -1;
        for (auto j : dataset.cities_with_stone (i))
          if (!matching.is_city_matched (j))
            if (k == -1 || tour.city_index (j) > tour.city_index (k))
              k = j;
        if (k == -1)
          continue;
        matching.match (i, k);
        auto new_score = evaluate (tour, matching);
        if (new_score.score > best_score.score) {
          best_score = new_score;
          stones.push_back (k);
        } else {
          matching.unmatch (i);
        }
      }
    }
  }
  auto last = std::remove_if (stones.begin (), stones.end (), [&] (int id) { return !matching.is_stone_matched (id); });
  stones.erase (last, stones.end ());
  while (timer.elapsed_ms () < allowed_ms)
    improve_round ();
  return {std::move (tour), std::move (matching)};
}
#include <iostream>
#include <numeric>
#include <random>
inline auto solve_no_tour (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> StoneMatching
{
  auto const timer = Timer ();
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto const weight_fn = [&] (int id) -> int { return dataset.stone (id).weight; };
  auto const value_fn = [&] (int id) -> long long { return dataset.stone (id).energy; };
  std::sort (indices.begin (), indices.end (), [&] (int a, int b) { return weight_fn (a) > weight_fn (b); });
  auto knapsack_result = Knapsack::knapsack (dataset.glove_capacity (), indices.begin (), indices.end (), weight_fn, value_fn);
  ASSERT (knapsack_result.exact == true);
  auto matching_result = [&] () {
    if (knapsack_result.selection.size () < dataset.num_cities ()) {
      // unweighted matching
      auto bip = BipartiteMatching::HopcroftKarp (knapsack_result.selection.size (), dataset.num_cities ());
      for (int i = 0; i < knapsack_result.selection.size (); ++i)
        for (auto j : dataset.cities_with_stone (knapsack_result.selection[i]))
          bip.add (i, j);
      auto matched = bip.solve ();
      auto result = std::vector<std::pair<int, int>> ();
      bip.for_each_edge ([&] (int from, int to) { result.emplace_back (knapsack_result.selection[from], to); });
      return result;
    } else {
      ASSERT (false);
      std::terminate ();
    }
  }();
  auto matching = StoneMatching (dataset);
  for (auto e : matching_result)
    matching.match (e.first, e.second);
  return matching;
}
#include <iostream>
#include <numeric>
#include <random>
inline auto solve_selection_only (Dataset const& dataset, std::mt19937& rng) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto result = Knapsack::knapsack (
    dataset.glove_capacity (),
    indices.begin (),
    indices.end (),
    [&] (int id) -> int { return dataset.stone (id).weight; },
    [&] (int id) -> long long { return dataset.stone (id).energy; });
  ASSERT (result.exact == true);
  return result.selection;
}
#include <iostream>
#include <numeric>
#include <random>
inline auto solve_single_matching (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const timer = Timer ();
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto const weight_fn = [&] (int id) -> int { return dataset.stone (id).weight; };
  auto const value_fn = [&] (int id) -> long long { return dataset.stone (id).energy; };
  auto selected = Knapsack::knapsack (dataset.glove_capacity (), indices.begin (), indices.end (), weight_fn, value_fn);
  ASSERT (selected.exact == true);
  auto matching = StoneMatching (dataset);
  for (auto i : selected.selection)
    if (matching.fits (dataset.stone (i).weight))
      matching.match (i, dataset.cities_with_stone (i).at (0));
  indices.resize (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  Tsp::tsp (
    indices.data (),
    indices.size (),
    dataset.starting_city (),
    [&] (int from, int to) { return dataset.distance (from, to); },
    rng,
    allowed_ms - timer.elapsed_ms ());
  auto tour = SimpleRoute (dataset, indices.data (), indices.data () + indices.size ());
  return {std::move (tour), std::move (matching)};
}
#include <algorithm>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>
inline auto solve_tsp_only (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> SimpleRoute
{
  auto indices = std::vector<int> (dataset.num_cities ());
  std::iota (indices.begin (), indices.end (), 0);
  auto cost = Tsp::tsp (
    indices.data (),
    indices.size (),
    dataset.starting_city (), //
    [&] (int from, int to) { return dataset.distance (from, to); },
    rng,
    allowed_ms);
  return SimpleRoute (dataset, indices.data (), indices.data () + indices.size ());
}
int main ()
{
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::steady_clock;
  auto rng = std::mt19937 (std::random_device {}());
  auto const timer = Timer ();
  std::ios_base::sync_with_stdio (false);
  std::cin.tie (0);
  std::cout.tie (0);
#ifdef EVAL
  auto is = fopen ("input.txt", "r");
  auto os = fopen ("output.txt", "w");
#else
  auto is = stdin;
  auto os = stdout;
#endif
  auto const data = read_dataset (is);
  auto const tour_does_not_matter = [&] () {
    if (data.glove_resistance () == 0.0)
      return true;
    for (int i = 0; i < data.num_cities (); ++i)
      for (int j = 0; j < i; ++j)
        if (data.distance (i, j) != data.distance (0, 1))
          return false;
    return true;
  }();
  auto const stones_dont_matter = [&] () {
    if (data.glove_capacity () == 0)
      return true;
    for (int i = 0; i < data.num_stones (); ++i)
      if (data.cities_with_stone (i).size () > 0)
        return false;
    return true;
  }();
  auto const only_one_matching = [&] () {
    for (int i = 0; i < data.num_stones (); ++i)
      if (data.cities_with_stone (i).size () > 1)
        return false;
    return true;
  }();
  // =============
  if (tour_does_not_matter && only_one_matching) {
    auto selected = solve_selection_only (data, rng);
    auto route = SimpleRoute (data);
    auto matching = StoneMatching (data);
    for (auto i : selected)
      matching.match (i, data.cities_with_stone (i).at (0));
    write_output (os, route, matching);
    return 0;
  }
  if (stones_dont_matter) {
    // find a good tour
    auto tour = solve_tsp_only (data, rng, 4900.0 - timer.elapsed_ms ());
    auto matching = StoneMatching (data);
    write_output (os, tour, matching);
    return 0;
  }
  if (tour_does_not_matter) {
    // find a complete matching and selection
    auto matching = solve_no_tour (data, rng, 4900.0 - timer.elapsed_ms ());
    auto route = SimpleRoute (data);
    write_output (os, route, matching);
    return 0;
  }
  if (only_one_matching) {
    // find a good selection and tour
    auto sol = solve_single_matching (data, rng, 4900.0 - timer.elapsed_ms ());
    write_output (os, sol.first, sol.second);
    return 0;
  }
  auto sol = solve_general (data, rng, 4900.0 - timer.elapsed_ms ());
  write_output (os, sol.first, sol.second);
}
