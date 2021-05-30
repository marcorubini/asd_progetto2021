#pragma once
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
