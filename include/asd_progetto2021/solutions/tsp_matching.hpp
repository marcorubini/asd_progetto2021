#pragma once
#include <algorithm>
#include <asd_progetto2021/utilities/bipartite_matching.hpp>
#include <asd_progetto2021/utilities/evaluation.hpp>
#include <asd_progetto2021/utilities/matching.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <asd_progetto2021/utilities/tsp.hpp>
#include <numeric>
#include <random>

using Matching = std::vector<std::pair<int, int>>;
using Edge = std::pair<int, int>;

inline auto greedy_select (Matching const& matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();

  auto result = StoneMatching (dataset);
  auto score = evaluate (route, result);
  for (auto e : matching) {
    if (result.is_city_matched (e.second))
      continue;
    if (result.is_stone_matched (e.first))
      continue;
    auto weight = dataset.stone (e.first).weight;
    if (weight + result.weight () > dataset.glove_capacity ())
      continue;
    result.match (e.first, e.second);
    auto new_score = evaluate (route, result);
    if (new_score.score < score.score) {
      result.unmatch_stone (e.first);
    } else {
      score = new_score;
    }
  }
  return result;
}

inline auto select_subset_heuristic_energy (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();

  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    return dataset.stone (e1.first).energy > dataset.stone (e2.first).energy;
  };

  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}

inline auto select_subset_heuristic_weight (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();

  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    return dataset.stone (e1.first).weight < dataset.stone (e2.first).weight;
  };

  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}

inline auto select_subset_heuristic_random (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto rng = std::mt19937 (std::random_device {}());
  auto const& dataset = route.dataset ();

  auto edges = dataset.stone_edges ();
  std::shuffle (edges.begin (), edges.end (), rng);
  return greedy_select (matching, route);
}

inline auto select_subset_heuristic_ratio (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();

  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    auto energy1 = dataset.stone (e1.first).energy;
    auto energy2 = dataset.stone (e2.first).energy;

    auto weight1 = dataset.stone (e1.first).weight;
    auto weight2 = dataset.stone (e2.first).weight;

    return energy1 / (weight1 + 1) > energy2 / (weight2 + 1);
  };

  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}

inline auto select_subset_heuristic_heavy (Matching matching, SimpleRoute const& route) -> StoneMatching
{
  auto const& dataset = route.dataset ();

  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    double energy1 = dataset.stone (e1.first).energy;
    double energy2 = dataset.stone (e2.first).energy;

    auto weight1 = dataset.stone (e1.first).weight;
    auto weight2 = dataset.stone (e2.first).weight;

    auto c1 = route.city_index (e1.second);
    auto c2 = route.city_index (e2.second);

    auto d1 = (dataset.num_cities () - c1);
    auto d2 = (dataset.num_cities () - c2);

    auto x = energy1 * 100 - d1 * weight1;
    auto y = energy2 * 100 - d2 * weight2;
    return x > y;
  };

  std::sort (edges.begin (), edges.end (), compare);
  return greedy_select (matching, route);
}

// ---

inline auto make_matching_greedy1 (Dataset const& dataset, SimpleRoute const&) -> std::vector<std::pair<int, int>>
{
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    auto stone1 = dataset.stone (e1.first);
    auto stone2 = dataset.stone (e2.first);
    return stone1.energy / stone1.weight > stone2.energy / stone2.weight;
  };

  std::sort (edges.begin (), edges.end (), compare);

  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto result = std::vector<std::pair<int, int>> ();
  for (auto e : edges) {
    if (taken_stones[e.first])
      continue;
    if (taken_cities[e.second])
      continue;
    result.push_back (e);
    taken_stones[e.first] = true;
    taken_cities[e.second] = true;
  }

  return result;
}

inline auto make_matching_greedy2 (Dataset const& dataset, SimpleRoute const&) -> std::vector<std::pair<int, int>>
{
  auto edges = dataset.stone_edges ();
  auto compare = [&] (Edge e1, Edge e2) {
    auto stone1 = dataset.stone (e1.first);
    auto stone2 = dataset.stone (e2.first);
    return stone1.weight < stone2.weight;
  };

  std::sort (edges.begin (), edges.end (), compare);

  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto result = std::vector<std::pair<int, int>> ();
  for (auto e : edges) {
    if (taken_stones[e.first])
      continue;
    if (taken_cities[e.second])
      continue;
    result.push_back (e);
    taken_stones[e.first] = true;
    taken_cities[e.second] = true;
  }

  return result;
}

inline auto make_matching_mcbm (Dataset const& dataset, SimpleRoute const& route) -> std::vector<std::pair<int, int>>
{
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto edges = std::vector<std::pair<int, int>> ();

  auto augment = [&] (int bound) {
    auto bip = BipartiteMatching (dataset.num_stones (), dataset.num_cities ());
    for (int i = dataset.num_cities () - 1; i >= 0; --i) {
      int city = route.at (i);

      if (taken_cities[city])
        continue;

      for (int stone_id : dataset.stones_at_city (city)) {
        if (taken_stones[stone_id])
          continue;

        int energy = dataset.stone (stone_id).energy;

        if (energy < bound)
          continue;

        bip.add (stone_id, city);
      }
    }

    for (auto e : bip.matching ()) {
      edges.push_back (e);
      taken_stones[e.first] = true;
      taken_cities[e.second] = true;
    }
  };

  auto bound = 0;
  for (auto e : dataset.stones ())
    bound = std::max (bound, e.energy);

  int full = std::min (dataset.num_cities (), dataset.num_stones ());
  while (bound > 0 && (int)edges.size () < full) {
    augment (bound);
    bound -= (bound / 200) + 1;
  }

  return edges;
}

inline auto make_matching_mcbm_weight (Dataset const& dataset, SimpleRoute const& route)
  -> std::vector<std::pair<int, int>>
{
  auto taken_stones = std::vector<bool> (dataset.num_stones ());
  auto taken_cities = std::vector<bool> (dataset.num_cities ());
  auto edges = std::vector<std::pair<int, int>> ();

  auto augment = [&] (int bound) {
    auto bip = BipartiteMatching (dataset.num_stones (), dataset.num_cities ());
    for (int i = dataset.num_cities () - 1; i >= 0; --i) {
      int city = route.at (i);

      if (taken_cities[city])
        continue;

      for (int stone_id : dataset.stones_at_city (city)) {
        if (taken_stones[stone_id])
          continue;

        int weight = dataset.stone (stone_id).weight;

        if (weight > bound)
          continue;

        bip.add (stone_id, city);
      }
    }

    for (auto e : bip.matching ()) {
      edges.push_back (e);
      taken_stones[e.first] = true;
      taken_cities[e.second] = true;
    }
  };

  auto bound = (int)(1u << 30);
  for (auto e : dataset.stones ())
    bound = std::min (bound, e.weight);

  int full = std::min (dataset.num_cities (), dataset.num_stones ());
  while (bound < (1u << 30) && (int)edges.size () < full) {
    augment (bound);
    bound += (bound / 200) + 1;
  }

  return edges;
}

// ---

inline auto final_improve (StoneMatching& stones, SimpleRoute& route) -> double
{
  auto const& dataset = route.dataset ();
  auto score = evaluate (route, stones);
  auto total = 0.0;
  for (int i = 1; i < dataset.num_cities (); ++i) {
    for (int j = i + 1; j < i + 10 && j < dataset.num_cities (); ++j) {
      route.reverse (i, j);
      auto new_score = evaluate (route, stones);
      if (new_score.score > score.score) {
        total += new_score.score - score.score;
        score = new_score;
      } else {
        route.reverse (i, j);
      }
    }
    for (int j = dataset.num_cities () - 1; j > i && j > dataset.num_cities () - 10; --j) {
      route.reverse (i, j);
      auto new_score = evaluate (route, stones);
      if (new_score.score > score.score) {
        total += new_score.score - score.score;
        score = new_score;
      } else {
        route.reverse (i, j);
      }
    }
  }
  return total;
}

inline auto final_improve_random (StoneMatching const& stones, SimpleRoute& route, std::mt19937& rng, int rounds) -> double
{
  auto const& dataset = route.dataset ();
  auto score = evaluate (route, stones);
  double tot = 0;
  while (rounds-- > 0) {
    int idx1 = (rng () % (dataset.num_cities () - 1) + 1);
    int idx2 = (rng () % (dataset.num_cities () - 1) + 1);

    if (idx1 > idx2)
      std::swap (idx1, idx2);

    route.reverse (idx1, idx2);
    auto new_score = evaluate (route, stones);
    if (new_score.score > score.score) {
      tot += new_score.score - score.score;
      score = new_score;
    } else {
      route.reverse (idx1, idx2);
    }
  }
  return tot;
}

inline auto print_statistics (Matching const& matching, Dataset const& dataset) -> void
{
  long long energy = 0;
  int weight = 0;
  for (auto e : matching) {
    auto s = dataset.stone (e.first);
    energy += s.energy;
    weight += s.weight;
  }

  std::cerr << "energy: " << energy << "\n";
  std::cerr << "weight: " << weight << "\n";
}

inline auto solve_tsp_matching (Dataset const& dataset) -> std::pair<SimpleRoute, StoneMatching>
{
  // compute a tsp solution
  auto rng = std::mt19937 (std::random_device {}());

  auto route = tsp_solve (dataset, rng, std::chrono::milliseconds (1000));
  auto matching = StoneMatching (dataset);
  auto score = evaluate (route, matching);

  auto const matching_strategies = {make_matching_greedy1, //
    make_matching_mcbm,
    make_matching_mcbm_weight};
  auto const subset_strategies = {select_subset_heuristic_energy, //
    select_subset_heuristic_random,
    select_subset_heuristic_ratio,
    select_subset_heuristic_weight,
    select_subset_heuristic_heavy};

  for (auto matching_strategy : matching_strategies) {
    auto match = matching_strategy (dataset, route);

    for (auto subset_strategy : subset_strategies) {
      auto curr = subset_strategy (match, route);
      auto new_score = evaluate (route, curr);

      if (new_score.score > score.score) {
        score = new_score;
        matching = std::move (curr);
      }
    }
  }

  auto improve = final_improve_random (matching, route, rng, 1000);
  return {std::move (route), std::move (matching)};
}