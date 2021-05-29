#pragma once
#include <asd_progetto2021/utilities/assert.hpp>
#include <asd_progetto2021/utilities/limits.hpp>
#include <asd_progetto2021/utilities/stone.hpp>

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

  auto sort () -> void
  {
    for (auto& vec : _stones_per_city)
      std::sort (vec.begin (), vec.end ());

    for (auto& vec : _cities_with_stone)
      std::sort (vec.begin (), vec.end ());

    std::sort (_stone_city_edges.begin (), _stone_city_edges.end ());
  }

  auto has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return std::binary_search (_stones_per_city[city_id].begin (), _stones_per_city[city_id].end (), stone_id);
  }
};
