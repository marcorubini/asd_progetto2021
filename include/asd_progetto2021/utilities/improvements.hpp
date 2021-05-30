#pragma once
#include <asd_progetto2021/utilities/evaluation.hpp>
#include <asd_progetto2021/utilities/matching.hpp>
#include <asd_progetto2021/utilities/route.hpp>
#include <chrono>
#include <random>

inline auto improve_tour_random (SimpleRoute& route, StoneMatching& matching, std::mt19937& rng) -> double
{
  auto const& data = route.dataset ();
  auto const n = data.num_cities ();
  auto x = rng () % (n - 1) + 1;
  auto y = rng () % (n - 1) + 1;
  if (x > y)
    std::swap (x, y);

  auto eval1 = evaluate (route, matching);
  route.reverse (x, y);
  auto eval2 = evaluate (route, matching);
  if (eval2.score < eval1.score) {
    route.reverse (x, y);
    return 0.0;
  } else {
    return eval2.score - eval1.score;
  }
}

inline auto improve_matching_random (SimpleRoute& route, StoneMatching& matching, std::mt19937& rng) -> double
{
  auto const& data = route.dataset ();

  if (data.num_stone_edges () == 0)
    return 0.0;

  auto const edge = data.stone_edges ()[rng () % data.num_stone_edges ()];
  auto const stone_id = edge.first;
  auto const city_id = edge.second;
  auto const eval1 = evaluate (route, matching);

  if (matching.is_stone_matched (stone_id) && matching.matched_city (stone_id) == city_id) {
    matching.unmatch_stone (stone_id);
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score)
      return eval2.score - eval1.score;
    matching.match (stone_id, city_id);
    return 0.0;
  }

  if (matching.is_stone_matched (stone_id) && matching.is_city_matched (city_id)) {
    auto const other_stone_id = matching.matched_stone (city_id);
    auto const other_city_id = matching.matched_city (stone_id);

    matching.unmatch_stone (stone_id);
    matching.unmatch_city (city_id);

    if (data.stone (stone_id).weight + matching.weight () <= data.glove_capacity ()) {
      matching.match (stone_id, city_id);
    }

    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      matching.match (stone_id, other_city_id);
      matching.match (other_stone_id, city_id);
      return 0.0;
    }

  } else if (matching.is_stone_matched (stone_id)) {
    auto const other_city_id = matching.matched_city (stone_id);
    matching.unmatch_stone (stone_id);

    if (matching.weight () + data.stone (stone_id).weight <= data.glove_capacity ())
      matching.match (stone_id, city_id);

    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      matching.match (stone_id, other_city_id);
      return 0.0;
    }
  } else if (matching.is_city_matched (city_id)) {
    auto const other_stone_id = matching.matched_stone (city_id);
    matching.unmatch_city (city_id);

    if (matching.weight () + data.stone (stone_id).weight <= data.glove_capacity ())
      matching.match (stone_id, city_id);
    auto const eval2 = evaluate (route, matching);

    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      matching.match (other_stone_id, city_id);
      return 0.0;
    }
  } else {
    if (data.stone (stone_id).weight + matching.weight () <= data.glove_capacity ())
      matching.match (stone_id, city_id);
    auto const eval2 = evaluate (route, matching);
    if (eval2.score > eval1.score) {
      return eval2.score - eval1.score;
    } else {
      if (matching.is_stone_matched (stone_id))
        matching.unmatch_stone (stone_id);
      return 0.0;
    }
  }
}

inline auto improve (SimpleRoute& route,
  StoneMatching& matching, //
  std::mt19937& rng,
  std::chrono::milliseconds remaining) -> double
{
  auto const time_start = std::chrono::steady_clock::now ();
  auto const elapsed = [time_start] () {
    auto now = std::chrono::steady_clock::now ();
    return std::chrono::duration_cast<std::chrono::milliseconds> (now - time_start);
  };

  auto improved = improve_matching_random (route, matching, rng);
  while (elapsed () < remaining) {
    improved += improve_matching_random (route, matching, rng);
    improved += improve_tour_random (route, matching, rng);
  }
  return improved;
}