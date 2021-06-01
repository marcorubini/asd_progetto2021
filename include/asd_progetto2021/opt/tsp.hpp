#pragma once
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

#include <asd_progetto2021/utilities/assert.hpp>

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