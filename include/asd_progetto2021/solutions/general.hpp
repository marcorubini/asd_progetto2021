#pragma once
#include <asd_progetto2021/dataset/evaluation.hpp>
#include <asd_progetto2021/dataset/stone_matching.hpp>
#include <asd_progetto2021/dataset/tour.hpp>
#include <asd_progetto2021/opt/bipartite_matching.hpp>
#include <asd_progetto2021/opt/knapsack.hpp>
#include <asd_progetto2021/utilities/timer.hpp>

#include <cmath>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>

inline auto select_knapsack (Dataset const& dataset) -> std::vector<int>
{
  auto indices = std::vector<int> (dataset.num_stones ());
  std::iota (indices.begin (), indices.end (), 0);

  auto const weight_fn = [&] (int id) -> int { return dataset.stone (id).weight; };
  auto const value_fn = [&] (int id) -> long long { return dataset.stone (id).energy; };

  return Knapsack::knapsack (dataset.glove_capacity (), indices.begin (), indices.end (), weight_fn, value_fn).selection;
}

inline auto find_matching (Dataset const& dataset, std::vector<int> selection) -> std::vector<std::pair<int, int>>
{
  std::sort (selection.begin (), selection.end (), [&] (int a, int b) {
    return dataset.stone (a).energy > dataset.stone (b).energy;
  });

  if (selection.size () > dataset.num_cities ())
    selection.resize (dataset.num_cities ());

  auto bip = BipartiteMatching::HopcroftKarp (selection.size (), dataset.num_cities ());

  for (int i = 0; i < (int)selection.size (); ++i)
    for (auto j : dataset.cities_with_stone (selection[i]))
      bip.add (i, j);

  auto matched = bip.solve ();
  auto result = std::vector<std::pair<int, int>> ();

  ASSERT (matched == selection.size ());

  bip.for_each_edge ([&] (int from, int to) { result.emplace_back (selection[from], to); });
  return result;
}

inline auto find_matching_heavy (SimpleRoute const& tour, std::vector<int> selection) -> std::vector<std::pair<int, int>>
{
  auto const& dataset = tour.dataset ();

  std::sort (selection.begin (), selection.end (), [&] (int a, int b) {
    return dataset.stone (a).energy > dataset.stone (b).energy;
  });

  if (selection.size () > dataset.num_cities ())
    selection.resize (dataset.num_cities ());

  auto bip = BipartiteMatching::WeightedMaxCardinalityBipartiteMatching (dataset.num_stones (), dataset.num_cities ());
  for (auto i : selection)
    for (auto j : dataset.cities_with_stone (i)) {
      auto w = dataset.stone (i).weight;
      auto c = tour.city_index (j);
      auto k = dataset.num_cities () - c + 1;
      bip.add (i, j, std::min (1ll * k * w, 10000000ll));
    }

  auto matched = bip.solve ().first;
  auto result = std::vector<std::pair<int, int>> ();

  ASSERT (matched == selection.size ());

  bip.for_each_match ([&] (int from, int to) { result.emplace_back (from, to); });

  return result;
}

inline auto solve_general (Dataset const& dataset, std::mt19937& rng, double allowed_ms) -> std::pair<SimpleRoute, StoneMatching>
{
  auto const timer = Timer ();

  auto tour = [&] () {
    auto indices = std::vector<int> (dataset.num_cities ());
    std::iota (indices.begin (), indices.end (), 0);
    auto const length = Tsp::tsp (
      indices.data (),
      indices.size (),
      dataset.starting_city (),
      [&] (int x, int y) { return dataset.distance (x, y); },
      rng,
      allowed_ms * 0.45);

    return SimpleRoute (dataset, indices.data (), indices.data () + indices.size ());
  }();

  auto matching = StoneMatching (dataset);
  auto best_score = Evaluation {-1e9, 0, 0};

  for (auto select_strategy : {select_knapsack}) {
    auto found = std::vector<std::pair<int, int>> ();
    auto selected = select_strategy (dataset);

    if (selected.size () <= 170) {
      found = find_matching_heavy (tour, std::move (selected));
    } else {
      found = find_matching (dataset, select_strategy (dataset));
    }
    auto curr = StoneMatching (dataset);
    for (auto e : found)
      curr.match (e.first, e.second);
    auto new_score = evaluate (tour, curr);
    if (new_score.score > best_score.score) {
      matching = curr;
      best_score = new_score;
    }
  }

  auto stones = std::vector<int> ();
  for (int i = 0; i < dataset.num_stones (); ++i)
    if (matching.is_stone_matched (i))
      stones.push_back (i);

  std::sort (stones.begin (), stones.end (), [&] (int x, int y) {
    return dataset.stone (x).weight > dataset.stone (y).weight;
  });

  auto const improve_stone_pair = [&] (int x, int y) {
    auto c1 = matching.matched_city (x);
    auto c2 = matching.matched_city (y);

    if (tour.city_index (c1) > tour.city_index (c2)) {
      std::swap (x, y);
      std::swap (c1, c2);
    }

    if (dataset.stone (x).weight < dataset.stone (y).weight)
      return;

    if (dataset.city_has_stone (c1, y) && dataset.city_has_stone (c2, x)) {
      matching.unmatch (x);
      matching.unmatch (y);
      matching.match (x, c2);
      matching.match (y, c1);
    }
  };

  auto const improve_stone_pos = [&] (int x) {
    for (auto c2 : dataset.cities_with_stone (x)) {
      auto c1 = matching.matched_city (x);
      if (!matching.is_city_matched (c2) && tour.city_index (c2) > tour.city_index (c1)) {
        matching.unmatch (x);
        matching.match (x, c2);
      }
    }
  };

  auto const improve_reverse = [&] (int left, int right) {
    tour.reverse (left, right);
    auto new_score = evaluate (tour, matching);
    if (new_score.score > best_score.score) {
      best_score = new_score;
    } else {
      tour.reverse (left, right);
    }
  };

  auto const improve_swap = [&] (int left, int right) {
    tour.swap (left, right);
    auto new_score = evaluate (tour, matching);
    if (new_score.score > best_score.score) {
      best_score = new_score;
    } else {
      tour.swap (left, right);
    }
  };

  best_score = evaluate (tour, matching);
  auto initial_score = best_score;

  auto indices = std::vector<int> ();
  int iters = 0;
  auto const improve_round = [&] () {
    ++iters;

    for (int i = 0; i < 20; ++i) {
      int x = rng () % stones.size ();
      int y = rng () % stones.size ();
      if (x != y) {
        improve_stone_pair (stones[x], stones[y]);
      }
    }

    for (int i = 0; i < 20; ++i) {
      int x = rng () % (dataset.num_cities () - 1) + 1;
      int y = rng () % (dataset.num_cities () - 1) + 1;
      if (y < x)
        std::swap (x, y);
      if (x != y) {
        improve_reverse (x, y);
      }
    }
  };

  while (timer.elapsed_ms () < allowed_ms * 0.95)
    improve_round ();
  best_score = evaluate (tour, matching);

  for (auto s : stones) {
    auto c = matching.matched_city (s);
    matching.unmatch (s);
    auto new_score = evaluate (tour, matching);
    if (new_score.score > best_score.score) {
      best_score = new_score;
    } else {
      matching.match (s, c);
    }
  }
  for (auto s : stones)
    if (matching.is_stone_matched (s))
      improve_stone_pos (s);
  for (int i = 0; i < dataset.num_stones (); ++i) {
    if (!matching.is_stone_matched (i)) {
      if (matching.fits (dataset.stone (i).weight)) {
        auto k = -1;
        for (auto j : dataset.cities_with_stone (i))
          if (!matching.is_city_matched (j))
            if (k == -1 || tour.city_index (j) > tour.city_index (k))
              k = j;
        if (k == -1)
          continue;
        matching.match (i, k);
        auto new_score = evaluate (tour, matching);
        if (new_score.score > best_score.score) {
          best_score = new_score;
          stones.push_back (k);
        } else {
          matching.unmatch (i);
        }
      }
    }
  }

  auto last = std::remove_if (stones.begin (), stones.end (), [&] (int id) { return !matching.is_stone_matched (id); });
  stones.erase (last, stones.end ());
  while (timer.elapsed_ms () < allowed_ms)
    improve_round ();

  return {std::move (tour), std::move (matching)};
}