#pragma once
#include <asd_progetto2021/dataset/dataset.hpp>
#include <asd_progetto2021/opt/tsp.hpp>

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
