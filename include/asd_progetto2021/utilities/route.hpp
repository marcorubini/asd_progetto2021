#pragma once
#include <asd_progetto2021/utilities/dataset.hpp>
#include <asd_progetto2021/utilities/fenwick_tree.hpp>

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
