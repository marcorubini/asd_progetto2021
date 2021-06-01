#pragma once
#include <vector>

#include <asd_progetto2021/utilities/assert.hpp>

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

  /*
    struct Dinic
    {
      using index_type = std::int16_t;
      using flow_type = std::int8_t;

      struct Edge
      {
        index_type from;
        index_type to;
        flow_type flow;
        flow_type capacity;

        Edge () = default;

        Edge (int from, int to, int flow, int capacity) //
          : from (from), to (to), flow (flow), capacity (capacity)
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
        std::vector<int> outgoing_edges;
        std::vector<int>::iterator next_edge;
        int level;
      };

      std::vector<Edge> edges;
      std::vector<Node> nodes;

      VisitSet visited;
      DoubleBuffer queue;

      Dinic (int N)    //
        : edges (),    //
          nodes (N),   //
          visited (N), //
          queue ()
      {}

      auto add (int from, int to, int capacity) -> void
      {
        edges.push_back ({from, to, 0, capacity});
        edges.push_back ({to, from, 0, 0});

        nodes[from].outgoing_edges.push_back (edges.size () - 2);
        nodes[to].outgoing_edges.push_back (edges.size () - 1);
      }

      auto bfs (int src, int sink) -> bool
      {
        visited.reset ();
        queue.clear ();

        visited.visit (src);
        nodes[src].level = 0;
        queue.push (src);

        while (!queue.empty ()) {
          auto const curr = queue.back ();
          queue.pop ();
          for (int edge_id : nodes[curr].outgoing_edges) {
            auto const edge = edges[edge_id];
            if (!edge.saturated () && !visited.visited (edge.to)) {
              visited.visit (edge.to);
              nodes[edge.to].level = nodes[curr].level + 1;
              queue.push (edge.to);
            }
          }
        }
        return visited.visited (sink);
      }

      auto augment (int edge_id, int bottleneck) -> void
      {
        edges[edge_id].flow += bottleneck;
        edges[edge_id ^ 1].flow -= bottleneck;
      }

      auto dfs (int curr, int sink, int bottleneck) -> int
      {
        if (curr == sink)
          return bottleneck;

        while (nodes[curr].next_edge != nodes[curr].outgoing_edges.end ()) {
          auto const edge_id = *nodes[curr].next_edge++;
          auto const edge = edges[edge_id];
          if (!edge.saturated () && nodes[edge.to].level == nodes[curr].level + 1) {
            auto const pushed = dfs (edge.to, sink, std::min (bottleneck, edge.residual ()));
            if (pushed > 0) {
              augment (edge_id, pushed);
              return pushed;
            }
          }
        }
        return 0;
      }

      auto solve (int src, int sink) -> int
      {
        auto flow = 0;
        while (bfs (src, sink)) {
          for (auto& node : nodes)
            node.next_edge = node.outgoing_edges.begin ();

          auto pushed = dfs (src, sink, 1u << 30);
          while (pushed > 0) {
            flow += pushed;
            pushed = dfs (src, sink, 1u << 30);
          }
        }
        return flow;
      }

      template<class Fn>
      auto for_each_flow_edge (Fn fn) const -> void
      {
        for (auto e : edges)
          if (e.flow > 0)
            fn (e);
      }
    };
  */

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