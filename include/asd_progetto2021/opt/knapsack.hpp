#pragma once
#include <algorithm>
#include <numeric>
#include <vector>

#include <asd_progetto2021/utilities/assert.hpp>

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