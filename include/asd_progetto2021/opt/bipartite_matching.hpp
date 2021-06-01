#pragma once
#include <asd_progetto2021/opt/flow.hpp>

#include <algorithm>

namespace BipartiteMatching
{
  /*
  struct MaxCardinalityBipartiteMatching
  {
    Flow::Dinic dinic;
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

    MaxCardinalityBipartiteMatching (int size1, int size2) //
      : dinic (size1 + size2 + 2),                         //
        size1 (size1)
    {
      for (int i = 0; i < size1; ++i)
        dinic.add (source (), left (i), 1);
      for (int i = 0; i < size2; ++i)
        dinic.add (right (i), sink (), 1);
    }

    auto add (int from, int to) -> void
    {
      dinic.add (left (from), right (to), 1);
    }

    auto solve () -> int
    {
      return dinic.solve (source (), sink ());
    }

    template<class Fn>
    auto for_each_match (Fn fn) const -> void
    {
      dinic.for_each_flow_edge ([=] (Flow::Dinic::Edge edge) {
        if (edge.from != source () && edge.to != sink ())
          fn (rev_left (edge.from), rev_right (edge.to));
      });
    }
  };
*/

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