#pragma once
#include <asd_progetto2021/dataset/limits.hpp>
#include <asd_progetto2021/dataset/stone.hpp>
#include <asd_progetto2021/utilities/assert.hpp>

#include <algorithm>
#include <array>
#include <bitset>
#include <deque>
#include <vector>

// Stores information about all stones and in which cities they can be found.
struct StoneIndex
{
  using index_type = std::int16_t;

private:
  std::array<Stone, MAX_STONES> _stones {};
  std::vector<std::vector<index_type>> _cities_with_stone {};
  std::vector<std::bitset<MAX_STONES>> _city_has_stone {};
  int _num_stones {};
  int _num_cities {};

public:
  StoneIndex (int num_stones, int num_cities) //
    : _cities_with_stone (num_stones),        //
      _num_stones (num_stones),               //
      _num_cities (num_cities),               //
      _city_has_stone (num_cities)
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
    _cities_with_stone[stone_id].push_back (city_id);
    _city_has_stone[city_id].set (stone_id);
  }

  auto cities_with_stone (int stone_id) const -> std::vector<index_type> const&
  {
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _cities_with_stone[stone_id];
  }

  auto city_has_stone (int city_id, int stone_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < num_cities ());
    ASSERT (stone_id >= 0 && stone_id < num_stones ());
    return _city_has_stone[city_id].test (stone_id);
  }
};
