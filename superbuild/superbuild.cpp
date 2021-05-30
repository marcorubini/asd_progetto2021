#include <algorithm>
#include <chrono>
#include <fstream>
#include <numeric>
#include <random>
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
struct Quirks
{
  bool tour_does_not_matter;
  bool stones_dont_matter;
  bool single_matching;
  bool triangle_inequality;
  Quirks (bool tour_does_not_matter, //
    bool stones_dont_matter,         //
    bool single_matching,            //
    bool triangle_inequality)
    : tour_does_not_matter (tour_does_not_matter), //
      stones_dont_matter (stones_dont_matter),     //
      single_matching (single_matching),           //
      triangle_inequality (triangle_inequality)
  {}
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
  auto fits (int weight) const -> bool
  {
    return weight + this->weight () <= dataset ().glove_capacity ();
  }
};
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>
inline auto solve_selection_only_greedy_energy (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto compare = [&] (int a, int b) {
    auto s1 = dataset.stone (a);
    auto s2 = dataset.stone (b);
    return s1.energy > s2.energy;
  };
  std::sort (indices.begin (), indices.end (), compare);
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int x : indices) {
    auto s = dataset.stone (x);
    if (weight + s.weight <= dataset.glove_capacity ()) {
      weight += s.weight;
      result.push_back (x);
    }
  }
  return result;
}
inline auto solve_selection_only_greedy_weight (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto compare = [&] (int a, int b) {
    auto s1 = dataset.stone (a);
    auto s2 = dataset.stone (b);
    return s1.weight < s2.weight;
  };
  std::sort (indices.begin (), indices.end (), compare);
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int x : indices) {
    auto s = dataset.stone (x);
    if (weight + s.weight <= dataset.glove_capacity ()) {
      weight += s.weight;
      result.push_back (x);
    }
  }
  return result;
}
inline auto solve_selection_only_knapsack (Dataset const& dataset) -> std::vector<int>
{
  auto const n = dataset.num_stones ();
  auto const c = [&] () {
    int tot = 0;
    for (auto x : dataset.stones ())
      tot = std::min (tot + x.weight, dataset.glove_capacity ());
    return tot;
  }();
  auto dp = std::vector<int> (c + 1);
  auto taken = std::vector<std::vector<bool>> (n, std::vector<bool> (c + 1));
  for (int i = 0; i < n; ++i) {
    auto stone = dataset.stone (i);
    for (int j = c; j >= stone.weight; --j) {
      if (dp[j - stone.weight] + stone.energy > dp[j]) {
        dp[j] = dp[j - stone.weight] + stone.energy;
        taken[i][j] = true;
      }
    }
  }
  auto result = std::vector<int> ();
  int capacity = c;
  int energy = 0;
  for (int i = n - 1; i >= 0; --i) {
    if (taken[i][capacity]) {
      capacity -= dataset.stone (i).weight;
      energy += dataset.stone (i).energy;
      result.push_back (i);
    }
  }
  return result;
}
inline auto solve_selection_only_greedy_ratio (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);
  auto compare = [&] (int a, int b) {
    auto s1 = dataset.stone (a);
    auto s2 = dataset.stone (b);
    auto r1 = (double)s1.energy / (s1.weight + 1);
    auto r2 = (double)s2.energy / (s2.weight + 1);
    return r1 > r2;
  };
  std::sort (indices.begin (), indices.end (), compare);
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int x : indices) {
    auto s = dataset.stone (x);
    if (weight + s.weight <= dataset.glove_capacity ()) {
      weight += s.weight;
      result.push_back (x);
    }
  }
  return result;
}
inline auto solve_selection_only_small_weight (Dataset const& dataset) -> std::vector<int>
{
  auto result = std::vector<int> ();
  auto weight = 0;
  for (int i = 0; i < dataset.num_cities (); ++i)
    if (weight + dataset.stone (i).weight <= dataset.glove_capacity ()) {
      weight += dataset.stone (i).weight;
      result.push_back (i);
    }
  return result;
}
inline auto solve_selection_only (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> std::vector<int>
{
  auto const total_weight = [&] () {
    long long tot = 0;
    for (auto s : dataset.stones ())
      tot += s.weight;
    return tot;
  }();
  auto const constant_weight = [&] () {
    for (auto s : dataset.stones ())
      if (s.weight != dataset.stone (0).weight)
        return false;
    return true;
  }();
  auto const constant_energy = [&] () {
    for (auto s : dataset.stones ())
      if (s.energy != dataset.stone (0).energy)
        return false;
    return true;
  }();
  auto const real_capacity = std::min (total_weight, (long long)dataset.glove_capacity ());
  if (total_weight <= dataset.glove_capacity ()) {
    return solve_selection_only_small_weight (dataset);
  }
  if (constant_energy) {
    return solve_selection_only_greedy_weight (dataset);
  }
  if (constant_weight) {
    return solve_selection_only_greedy_energy (dataset);
  }
  if (real_capacity * dataset.num_stones () <= 10000000) {
    return solve_selection_only_knapsack (dataset);
  }
  auto const sum_energy = [&dataset] (std::vector<int> const& selection) {
    auto energy = 0ll;
    for (auto i : selection)
      energy += dataset.stone (i).energy;
    return energy;
  };
  auto const strategies = {//
    solve_selection_only_greedy_weight,
    solve_selection_only_greedy_energy,
    solve_selection_only_greedy_ratio};
  auto result = solve_selection_only_small_weight (dataset);
  auto result_energy = sum_energy (result);
  for (auto s : strategies) {
    auto curr = s (dataset);
    auto curr_energy = sum_energy (curr);
    if (curr_energy > result_energy) {
      result = curr;
      result_energy = curr_energy;
    }
  }
  return result;
}
#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
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
  int solve (int src, int sink)
  {
    int max_flow = 0;
    while (bfs (src, sink)) {
      std::fill (begin (next_edge), end (next_edge), 0);
      for (int flow = dfs (src, sink, 1u << 30); flow != 0; flow = dfs (src, sink, 1u << 30)) {
        max_flow += flow;
      }
    }
    return max_flow;
  }
  auto previous_edges () -> std::vector<std::tuple<int, int, int>>
  {
    auto result = std::vector<std::tuple<int, int, int>> ();
    for (auto e : edges)
      if (std::get<3> (e) > 0)
        result.emplace_back (std::get<0> (e), std::get<1> (e), std::get<3> (e));
    return result;
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
  auto matching () -> int
  {
    return dinic.solve (source (), sink ());
  }
  auto previous_edges () -> std::vector<std::pair<int, int>>
  {
    auto result = std::vector<std::pair<int, int>> ();
    for (auto e : dinic.previous_edges ())
      if (std::get<0> (e) != source () && std::get<1> (e) != sink ())
        result.emplace_back (std::get<0> (e) - 2, std::get<1> (e) - size1 - 2);
    return result;
  }
};
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>
inline auto solve_no_tour_incremental_matching (Dataset const& dataset, //
  std::vector<int> const& selected) -> StoneMatching
{
  auto bip = BipartiteMatching (dataset.num_stones (), dataset.num_cities ());
  auto const add_stone = [&] (int id) {
    for (int j : dataset.cities_with_stone (id))
      bip.add (id, j);
  };
  int last = 0;
  while (last < (int)selected.size () && last < dataset.num_cities ())
    add_stone (selected[last++]);
  auto const augment = [&] (int amount) {
    while (amount-- > 0 && last < (int)selected.size ())
      add_stone (selected[last++]);
    return bip.matching ();
  };
  auto flow = bip.matching ();
  auto edges = bip.previous_edges ();
  auto next_flow = augment (5);
  while (next_flow > 0) {
    flow += next_flow;
    edges = bip.previous_edges ();
    next_flow = augment (5);
  }
  auto result = StoneMatching (dataset);
  for (auto e : edges)
    result.match (e.first, e.second);
  return result;
}
inline auto solve_no_tour (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> StoneMatching
{
  auto const time_start = Clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<Milli> (Clock::now () - time_start);
  };
  if (dataset.num_stones () == 0) {
    return StoneMatching (dataset);
  }
  auto const total_weight = [&] () {
    int tot = 0;
    for (auto s : dataset.stones ())
      tot += s.weight;
    return tot;
  }();
  auto const constant_weight = [&] () {
    for (auto s : dataset.stones ())
      if (s.weight != dataset.stone (0).weight)
        return false;
    return true;
  }();
  auto const constant_energy = [&] () {
    for (auto s : dataset.stones ())
      if (s.energy != dataset.stone (0).energy)
        return false;
    return true;
  }();
  auto const real_capacity = std::min (total_weight, dataset.glove_capacity ());
  if (constant_weight || total_weight <= dataset.glove_capacity ()) {
    // find a selection that maximizes energy, then check for a saturating matching.
    auto selected = solve_selection_only_greedy_energy (dataset);
    return solve_no_tour_incremental_matching (dataset, selected);
  }
  if (constant_energy) {
    // find a selection that minimizes weight, then check for a saturating matching.
    auto selected = solve_selection_only_greedy_weight (dataset);
    return solve_no_tour_incremental_matching (dataset, selected);
  }
  if (real_capacity * dataset.num_stones () <= 10000000) {
    // find a selection that maximizes energy, then check for a saturating matching.
    auto selected = solve_selection_only_knapsack (dataset);
    std::sort (selected.begin (), selected.end (), [&] (int a, int b) {
      return dataset.stone (a).energy > dataset.stone (b).energy;
    });
    return solve_no_tour_incremental_matching (dataset, selected);
  }
  std::terminate ();
}
#include <algorithm>
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
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>
inline auto solve_tsp_only_bootstrap_greedy (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
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
    tour.swap (i, tour.city_index (curr_best));
  }
  return tour;
}
inline auto solve_tsp_only_bootstrap_random (Dataset const& dataset, std::mt19937& rng) -> SimpleRoute
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
inline auto solve_tsp_only_improve_random (SimpleRoute& route, std::mt19937& rng, int rounds, int upper) -> void
{
  auto const& dataset = route.dataset ();
  auto const n = dataset.num_cities ();
  while (rounds-- > 0) {
    int x = rng () % (n - 1) + 1;
    int y = rng () % (n - 1) + 1;
    if (x > y)
      std::swap (x, y);
    if (route.reverse_profit (x, y) <= -upper)
      route.reverse (x, y);
  }
}
inline auto solve_tsp_only_improve_random_scaled (SimpleRoute& route, std::mt19937& rng, int rounds, int upper) -> void
{
  for (int k = upper; k >= 1; k /= 2)
    solve_tsp_only_improve_random (route, rng, rounds, k);
}
inline auto solve_tsp_only_improve_all (SimpleRoute& route, std::mt19937& rng, int upper) -> void
{
  auto const& dataset = route.dataset ();
  auto const n = dataset.num_cities ();
  auto indices = std::vector<int> (n - 1);
  std::iota (indices.begin (), indices.end (), 1);
  std::shuffle (indices.begin (), indices.end (), rng);
  for (auto i : indices) {
    for (int j = i + 1; j < n; ++j) {
      if (route.reverse_profit (i, j) <= -upper)
        route.reverse (i, j);
    }
  }
}
inline auto solve_tsp_only_improve_all_scaled (SimpleRoute& route, std::mt19937& rng, int upper) -> void
{
  while (upper >= 1) {
    solve_tsp_only_improve_all (route, rng, upper);
    upper /= 2;
  }
}
inline auto solve_tsp_only (Dataset const& dataset, std::mt19937& rng, Milli remaining) -> SimpleRoute
{
  auto time_start = Clock::now ();
  auto const elapsed = [time_start] () //
  { return std::chrono::duration_cast<Milli> (Clock::now () - time_start); };
  auto result = solve_tsp_only_bootstrap_greedy (dataset, rng);
  solve_tsp_only_improve_all_scaled (result, rng, 4);
  solve_tsp_only_improve_all_scaled (result, rng, 2);
  solve_tsp_only_improve_random_scaled (result, rng, 10000, 2);
  while (elapsed () < remaining * 0.8) {
    auto temp = solve_tsp_only_bootstrap_random (dataset, rng);
    solve_tsp_only_improve_all_scaled (temp, rng, 4);
    solve_tsp_only_improve_all_scaled (temp, rng, 2);
    solve_tsp_only_improve_random_scaled (result, rng, 10000, 2);
    if (temp.length () < result.length ()) {
      result = temp;
    }
  }
  while (elapsed () < remaining)
    solve_tsp_only_improve_random (result, rng, 10000, 1);
  return result;
}
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
inline auto improve_tour_random (SimpleRoute& route, StoneMatching& matching, std::mt19937& rng) -> double
{
  auto const& data = route.dataset ();
  auto const n = data.num_cities ();
  auto x = rng () % (n - 1) + 1;
  auto y = rng () % (n - 1) + 1;
  if (x > y)
    std::swap (x, y);
  auto eval1 = evaluate (route, matching);
  route.reverse (x, y);
  auto eval2 = evaluate (route, matching);
  if (eval2.score < eval1.score) {
    route.reverse (x, y);
    return 0.0;
  } else {
    return eval2.score - eval1.score;
  }
}
inline auto improve_matching_random (SimpleRoute& route, StoneMatching& matching, std::mt19937& rng) -> double
{
  auto const& data = route.dataset ();
  if (data.num_stone_edges () == 0)
    return 0.0;
  auto const edge = data.stone_edges ()[rng () % data.num_stone_edges ()];
  auto const stone_id = edge.first;
  auto const city_id = edge.second;
  auto const eval1 = evaluate (route, matching);
  if (matching.is_stone_matched (stone_id) && matching.matched_city (stone_id) == city_id) {
    matching.unmatch_stone (stone_id);
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score)
      return eval2.score - eval1.score;
    matching.match (stone_id, city_id);
    return 0.0;
  }
  if (matching.is_stone_matched (stone_id) && matching.is_city_matched (city_id)) {
    auto const other_stone_id = matching.matched_stone (city_id);
    auto const other_city_id = matching.matched_city (stone_id);
    matching.unmatch_stone (stone_id);
    matching.unmatch_city (city_id);
    if (data.stone (stone_id).weight + matching.weight () <= data.glove_capacity ()) {
      matching.match (stone_id, city_id);
    }
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      matching.match (stone_id, other_city_id);
      matching.match (other_stone_id, city_id);
      return 0.0;
    }
  } else if (matching.is_stone_matched (stone_id)) {
    auto const other_city_id = matching.matched_city (stone_id);
    matching.unmatch_stone (stone_id);
    if (matching.weight () + data.stone (stone_id).weight <= data.glove_capacity ())
      matching.match (stone_id, city_id);
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      matching.match (stone_id, other_city_id);
      return 0.0;
    }
  } else if (matching.is_city_matched (city_id)) {
    auto const other_stone_id = matching.matched_stone (city_id);
    matching.unmatch_city (city_id);
    if (matching.weight () + data.stone (stone_id).weight <= data.glove_capacity ())
      matching.match (stone_id, city_id);
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      matching.match (other_stone_id, city_id);
      return 0.0;
    }
  } else {
    if (data.stone (stone_id).weight + matching.weight () <= data.glove_capacity ())
      matching.match (stone_id, city_id);
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      return 0.0;
    }
  }
}
inline auto improve (SimpleRoute& route,
  StoneMatching& matching, //
  std::mt19937& rng,
  std::chrono::milliseconds remaining) -> double
{
  auto const time_start = std::chrono::steady_clock::now ();
  auto const elapsed = [time_start] () {
    auto now = std::chrono::steady_clock::now ();
    return std::chrono::duration_cast<std::chrono::milliseconds> (now - time_start);
  };
  auto improved = improve_matching_random (route, matching, rng);
  while (elapsed () < remaining) {
    improved += improve_matching_random (route, matching, rng);
    improved += improve_tour_random (route, matching, rng);
  }
  return improved;
}
inline auto solve_general (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const time_start = Clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<Milli> (Clock::now () - time_start);
  };
  auto const total_weight = [&] () {
    long long tot = 0;
    for (auto s : dataset.stones ())
      tot += s.weight;
    return tot;
  }();
  auto const constant_weight = [&] () {
    for (auto s : dataset.stones ())
      if (s.weight != dataset.stone (0).weight)
        return false;
    return true;
  }();
  auto const constant_energy = [&] () {
    for (auto s : dataset.stones ())
      if (s.energy != dataset.stone (0).energy)
        return false;
    return true;
  }();
  auto const real_capacity = std::min (total_weight, (long long)dataset.glove_capacity ());
  // ---
  auto route = solve_tsp_only (dataset, rng, remaining / 5);
  if (total_weight <= dataset.glove_capacity () || constant_weight) {
    auto selected = solve_selection_only_greedy_energy (dataset);
    auto matched = solve_no_tour_incremental_matching (dataset, selected);
    auto improved = improve (route, matched, rng, remaining - elapsed ());
    return {std::move (route), std::move (matched)};
  }
  if (constant_energy) {
    auto selected = solve_selection_only_greedy_weight (dataset);
    auto matched = solve_no_tour_incremental_matching (dataset, selected);
    auto improved = improve (route, matched, rng, remaining - elapsed ());
    return {std::move (route), std::move (matched)};
  }
  if (real_capacity * dataset.num_stones () <= 10000000) {
    auto selected = solve_selection_only_knapsack (dataset);
    std::sort (selected.begin (), selected.end (), [&] (int a, int b) {
      return dataset.stone (a).energy > dataset.stone (b).energy;
    });
    auto matched = solve_no_tour_incremental_matching (dataset, selected);
    auto improved = improve (route, matched, rng, remaining - elapsed ());
    return {std::move (route), std::move (matched)};
  }
  auto const selection_strategies = {solve_selection_only_greedy_energy, //
    solve_selection_only_greedy_weight,
    solve_selection_only_greedy_ratio};
  auto matched = StoneMatching (dataset);
  auto score = evaluate (route, matched);
  for (auto strategy : selection_strategies) {
    auto selected = strategy (dataset);
    auto new_matched = solve_no_tour_incremental_matching (dataset, selected);
    auto eval = evaluate (route, new_matched);
    if (eval.score > score.score) {
      score = eval;
      matched = std::move (new_matched);
    }
  }
  auto improved = improve (route, matched, rng, remaining - elapsed ());
  return {std::move (route), std::move (matched)};
}
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>
inline auto solve_single_matching (Dataset const& dataset, //
  std::mt19937& rng,
  Milli remaining) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const time_start = Clock::now ();
  auto const elapsed = [time_start] () {
    return std::chrono::duration_cast<Milli> (Clock::now () - time_start);
  };
  auto const total_weight = [&] () {
    int tot = 0;
    for (auto s : dataset.stones ())
      tot += s.weight;
    return tot;
  }();
  auto const constant_weight = [&] () {
    for (auto s : dataset.stones ())
      if (s.weight != dataset.stone (0).weight)
        return false;
    return true;
  }();
  auto const constant_energy = [&] () {
    for (auto s : dataset.stones ())
      if (s.energy != dataset.stone (0).energy)
        return false;
    return true;
  }();
  auto const real_capacity = std::min (total_weight, dataset.glove_capacity ());
  // ----
  if (total_weight <= dataset.glove_capacity ()) {
    // take everything
    auto matching = StoneMatching (dataset);
    for (int i = 0; i < dataset.num_stones (); ++i)
      matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }
  if (constant_weight) {
    // maximize energy
    auto matching = StoneMatching (dataset);
    for (auto i : solve_selection_only_greedy_energy (dataset))
      if (matching.fits (dataset.stone (i).weight))
        matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }
  if (constant_energy) {
    // minimize weight
    auto matching = StoneMatching (dataset);
    for (auto i : solve_selection_only_greedy_weight (dataset))
      if (matching.fits (dataset.stone (i).weight))
        matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }
  if (real_capacity * dataset.num_stones () <= 10000000) {
    // knapsack
    auto matching = StoneMatching (dataset);
    for (auto i : solve_selection_only_knapsack (dataset))
      if (matching.fits (dataset.stone (i).weight))
        matching.match (i, dataset.cities_with_stone (i).at (0));
    auto r = ((remaining - elapsed ()) * 8) / 10;
    auto route = solve_tsp_only (dataset, rng, r);
    auto improved = improve (route, matching, rng, remaining - elapsed ());
    return {std::move (route), std::move (matching)};
  }
  std::terminate ();
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
  auto sample_size = 800000;
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
int main ()
{
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::steady_clock;
  auto rng = std::mt19937 (std::random_device {}());
  auto time_start = steady_clock::now ();
  auto const elapsed = [time_start] () {
    return duration_cast<milliseconds> (steady_clock::now () - time_start);
  };
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
  };
  auto const stones_dont_matter = [&] () {
    if (data.glove_capacity () == 0)
      return true;
    if (data.num_stone_edges () == 0)
      return true;
    return false;
  };
  auto const only_one_matching = [&] () {
    for (int i = 0; i < data.num_cities (); ++i)
      if (data.stones_at_city (i).size () > 1)
        return false;
    for (int i = 0; i < data.num_stones (); ++i)
      if (data.cities_with_stone (i).size () > 1)
        return false;
    return true;
  };
  auto const quirks = Quirks (tour_does_not_matter (), //
    stones_dont_matter (),
    only_one_matching (),
    false);
  // =============
  if (quirks.tour_does_not_matter && quirks.single_matching) {
    auto selected = solve_selection_only (data, rng, Milli (5000) - elapsed ());
    auto route = SimpleRoute (data);
    auto matching = StoneMatching (data);
    for (auto i : selected)
      matching.match (i, data.cities_with_stone (i).at (0));
    write_output (os, route, matching);
    return 0;
  }
  if (stones_dont_matter ()) {
    // find a good tour
    auto tour = solve_tsp_only (data, rng, Milli (5000) - elapsed ());
    auto matching = StoneMatching (data);
    write_output (os, tour, matching);
    return 0;
  }
  if (quirks.tour_does_not_matter) {
    // find a complete matching and selection
    auto matching = solve_no_tour (data, rng, Milli (5000) - elapsed ());
    auto route = SimpleRoute (data);
    write_output (os, route, matching);
    return 0;
  }
  if (quirks.single_matching) {
    // find a good selection and tour
    auto sol = solve_single_matching (data, rng, Milli (5000) - elapsed ());
    write_output (os, sol.first, sol.second);
    return 0;
  }
  auto sol = solve_general (data, rng, Milli (4950) - elapsed ());
  write_output (os, sol.first, sol.second);
}
