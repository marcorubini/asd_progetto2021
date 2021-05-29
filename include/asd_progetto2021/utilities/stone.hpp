#pragma once
#include <asd_progetto2021/utilities/assert.hpp>
#include <asd_progetto2021/utilities/limits.hpp>

struct Stone
{
  int weight {-1};
  int energy {-1};

  Stone () = default;

  Stone (int weight, int energy) : weight (weight), energy (energy)
  {
    ASSERT (weight >= 1 && weight <= MAX_STONE_WEIGHT);
    ASSERT (energy >= 1 && energy <= MAX_STONE_ENERGY);
  }

  bool operator== (Stone other) const
  {
    return weight == other.weight && energy == other.energy;
  }

  bool operator!= (Stone other) const
  {
    return weight != other.weight || energy != other.energy;
  }

  bool operator< (Stone other) const
  {
    return weight < other.weight || (weight == other.weight && energy < other.energy);
  }
};

// Returns true if lhs is better than rhs.
inline auto pareto_compare (Stone lhs, Stone rhs) -> bool
{
  if (lhs.energy != rhs.energy)
    return lhs.energy > rhs.energy && lhs.weight <= rhs.weight;
  return lhs.weight < rhs.weight;
}