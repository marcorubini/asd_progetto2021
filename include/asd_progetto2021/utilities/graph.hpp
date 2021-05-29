#pragma once
#include <asd_progetto2021/utilities/assert.hpp>
#include <asd_progetto2021/utilities/limits.hpp>

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
