#pragma once

#if 0
#  pragma once
#  include <asd_progetto2021/utilities/fenwick_tree.hpp>
#  include <asd_progetto2021/utilities/matching.hpp>
#  include <asd_progetto2021/utilities/route.hpp>

struct RouteMatch
{
private:
  std::reference_wrapper<Dataset const> _dataset;
  SimpleRoute _route;
  StoneMatching _matching;
  FenwickTree<MAX_CITIES> _prefix_weight {};

  double _travel_time = 0;

public:
  RouteMatch (Dataset const& dataset) : _dataset (dataset), _route (dataset), _matching (dataset)
  {}

  auto dataset () const -> Dataset const&
  {
    return _dataset.get ();
  }

  auto route () const -> SimpleRoute const&
  {
    return _route;
  }

  auto matching () const -> StoneMatching const&
  {
    return _matching;
  }

  auto energy () const -> int
  {
    return matching ().energy ();
  }

  auto weight () const -> int
  {
    return matching ().weight ();
  }

  auto length () const -> int
  {
    return route ().length ();
  }

  auto travel_time () const -> double
  {
    return _travel_time;
  }

  auto score () const -> double
  {
    return energy () - travel_time () * dataset ().glove_resistance ();
  }

  // ---

  auto is_city_matched (int city_id) const -> bool
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return matching ().is_city_matched (city_id);
  }

  auto is_stone_matched (int stone_id) const -> bool
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    return matching ().is_stone_matched (stone_id);
  }

  auto matched_city (int stone_id) const -> int
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    return matching ().matched_city (stone_id);
  }

  auto matched_stone (int city_id) const -> int
  {
    ASSERT (city_id >= 0 && city_id < dataset ().num_cities ());
    return matching ().matched_stone (city_id);
  }

  // weight prefix sum

  auto subroute_weight (int idx1, int idx2) const -> double
  {
    ASSERT (idx1 >= -1 && idx1 <= dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 <= dataset ().num_cities ());

    if (idx2 == dataset ().num_cities ())
      return weight () - _prefix_weight.prefix_sum (idx1);
    return _prefix_weight.sum (idx1, idx2 + 1);
  }

  auto prefix_weight (int last) const -> double
  {
    ASSERT (last >= 0 && last <= dataset ().num_cities ());
    return subroute_weight (0, last);
  }

  auto suffix_weight (int first) const -> double
  {
    return subroute_weight (first, dataset ().num_cities ());
  }

  auto city_index (int city_id) const -> int
  {
    ASSERT (city_id >= 0 && city_id <= dataset ().num_cities ());
    return _route.city_index (city_id);
  }

  auto at (int index) const -> int
  {
    ASSERT (index >= -1 && index <= dataset ().num_cities ());
    return _route.at (index);
  }

  // partial travel time evaluation

  auto subroute_travel_time (int idx1, int idx2) const -> double
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 <= dataset ().num_cities ());
    return subroute_travel_time (idx1, idx2, prefix_weight (idx1));
  }

  auto subroute_travel_time (int idx1, int idx2, double weight) const -> double
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 <= dataset ().num_cities ());
    auto curr_weight = prefix_weight (idx1) + weight;
    auto travel = 0.0;
    while (idx1 < idx2) {
      auto curr_city = at (idx1);
      if (is_city_matched (curr_city))
        curr_weight += dataset ().stone (matched_stone (curr_city)).weight;
      travel += dataset ().travel_time (curr_city, at (idx1 + 1), curr_weight);
    }
    return travel;
  }

  auto subroute_travel_time_delta (int idx1, int idx2, double weight) const -> double
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 <= dataset ().num_cities ());
    return subroute_travel_time (idx1, idx2, weight) - subroute_travel_time (idx1, idx2);
  }

  auto suffix_travel_time (int idx1) const -> double
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    return subroute_travel_time (idx1, dataset ().num_cities ());
  }

  auto suffix_travel_time (int idx1, double weight) const -> double
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    return subroute_travel_time (idx1, dataset ().num_cities (), weight);
  }

  auto suffix_travel_time_delta (int idx1, double weight) const -> double
  {
    ASSERT (idx1 >= 0 && idx1 <= dataset ().num_cities ());
    return subroute_travel_time_delta (idx1, dataset ().num_cities (), weight);
  }

  auto reverse_subroute_travel_delta (int idx1, int idx2) const -> double
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());

    if (idx1 == idx2)
      return 0;

    auto const t1 = subroute_travel_time (idx1, idx2);
    auto const w1 = prefix_weight (idx1);
    auto const w2 = prefix_weight (idx2 + 1);
    auto const outside_delta = dataset ().travel_time (at (idx1 - 1), at (idx2), w1) //
      - dataset ().travel_time (at (idx1 - 1), at (idx1), w1)                        //
      + dataset ().travel_time (at (idx1), at (idx2 + 1), w2)                        //
      - dataset ().travel_time (at (idx2), at (idx2 + 1), w2);

    auto const inside_time = subroute_travel_time (idx1, idx2);

    auto curr_weight = w1;
    auto travel = 0.0;
    while (idx2 > idx1) {
      auto curr_city = at (idx2);
      if (is_city_matched (curr_city))
        curr_weight += dataset ().stone (matched_stone (curr_city)).weight;
      travel += dataset ().travel_time (dataset ().distance (curr_city, at (idx2 - 1)), curr_weight);
      idx2--;
    }

    return outside_delta + travel - inside_time;
  }

  // ---

  auto match_profit (int stone_id, int city_id) const -> double
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (city_id >= 0 && city_id <= dataset ().num_cities ());
    ASSERT (!is_stone_matched (stone_id));
    ASSERT (!is_city_matched (city_id));

    auto stone_weight = dataset ().stone (stone_id).weight;
    auto stone_energy = dataset ().stone (stone_id).energy;
    auto index = city_index (city_id);

    ASSERT (weight () + stone_weight <= dataset ().glove_capacity ());

    return stone_energy - suffix_travel_time_delta (index, stone_weight) * dataset ().glove_resistance ();
  }

  auto match (int stone_id, int city_id) -> double
  {
    ASSERT (stone_id >= 0 && stone_id < dataset ().num_stones ());
    ASSERT (city_id >= 0 && city_id <= dataset ().num_cities ());
    ASSERT (!is_stone_matched (stone_id));
    ASSERT (!is_city_matched (city_id));

    auto stone_weight = dataset ().stone (stone_id).weight;
    auto stone_energy = dataset ().stone (stone_id).energy;
    auto index = city_index (city_id);

    ASSERT (weight () + stone_weight <= dataset ().glove_capacity ());

    auto delta = suffix_travel_time_delta (index, stone_weight);

    _matching.match (stone_id, city_id);
    _prefix_weight.add (index, stone_weight);
    return stone_energy - delta * dataset ().glove_resistance ();
  }

  // ---

  auto reverse_travel_time_delta (int idx1, int idx2) const -> double
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());

    if (idx1 == idx2)
      return 0.0;

    return reverse_subroute_travel_delta (idx1, idx2) * dataset ().glove_resistance ();
  }

  auto reverse (int idx1, int idx2) -> double
  {
    ASSERT (idx1 >= 1 && idx1 < dataset ().num_cities ());
    ASSERT (idx2 >= idx1 && idx2 < dataset ().num_cities ());

    if (idx1 == idx2)
      return 0.0;

    auto const profit = reverse_travel_time_delta (idx1, idx2);

    for (int i = idx1; i <= idx2; ++i)
      if (is_city_matched (at (i)))
        _prefix_weight.add (at (i), -dataset ().stone (at (i)).weight);

    _route.reverse (idx1, idx2);

    for (int i = idx1; i <= idx2; ++i)
      if (is_city_matched (at (i)))
        _prefix_weight.add (at (i), -dataset ().stone (at (i)).weight);

    _travel_time += profit;
    return profit * dataset ().glove_resistance ();
  }
};
#  enidf