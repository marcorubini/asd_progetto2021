#pragma once
#include <asd_progetto2021/utilities/dataset.hpp>
#include <functional>

struct StoneMatching
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  std::array<int, MAX_STONES> _matched_city;
  std::array<int, MAX_CITIES> _matched_stone;
  int _weight = 0;
  int _energy = 0;

public:
  StoneMatching (Dataset const& dataset) : _dataset (dataset)
  {
    std::fill (_matched_city.begin (), _matched_city.end (), -1);
    std::fill (_matched_stone.begin (), _matched_stone.end (), -1);
  }

  auto dataset () const -> Dataset const&
  {
    return _dataset.get ();
  }

  auto is_city_matched (int city_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return matched_stone (city_id) != -1;
  }

  auto is_stone_matched (int stone_id) const -> bool
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    return matched_city (stone_id) != -1;
  }

  auto matched_stone (int city_id) const -> int
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return _matched_stone[city_id];
  }

  auto matched_city (int stone_id) const -> int
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    return _matched_city[stone_id];
  }

  auto match (int stone_id, int city_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (!is_city_matched (city_id));
    ASSERT (!is_stone_matched (stone_id));
    _matched_city[stone_id] = city_id;
    _matched_stone[city_id] = stone_id;
    _weight += dataset ().stone (stone_id).weight;
    _energy += dataset ().stone (stone_id).energy;
  }

  auto unmatch_stone (int stone_id) -> void
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (is_stone_matched (stone_id));
    auto const city_id = matched_city (stone_id);
    _matched_city[stone_id] = -1;
    _matched_stone[city_id] = -1;
    _weight -= dataset ().stone (stone_id).weight;
    _energy -= dataset ().stone (stone_id).energy;
  }

  auto unmatch_city (int city_id) -> void
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    ASSERT (is_city_matched (city_id));
    auto const stone_id = matched_stone (city_id);
    _matched_stone[city_id] = -1;
    _matched_city[stone_id] = -1;
    _weight -= dataset ().stone (stone_id).weight;
    _energy -= dataset ().stone (stone_id).energy;
  }

  auto weight () const -> int
  {
    return _weight;
  }

  auto energy () const -> int
  {
    return _energy;
  }

  auto fits (int weight) const -> bool
  {
    return weight + this->weight () <= dataset ().glove_capacity ();
  }
};
