#pragma
#include <asd_progetto2021/dataset.hpp>

#include <random>

inline auto select_tour_tsp (Dataset const& dataset) -> Route
{
  auto const bootstrap = [&dataset] () -> Route {
    auto route = Route (dataset);
    auto rng = std::mt19937 (std::random_device {}());
    auto indices = std::vector<int> (dataset.num_cities ());
    std::iota (indices.begin (), indices.end (), 0);
    std::shuffle (indices.begin (), indices.end (), rng);

    while (route.size () != dataset.num_cities ()) {
      int curr_best = -1;
      int curr_dist = 1u << 30;
      for (int i : indices) {
        if (!route.is_visited (i) && dataset.distance (i, route.back ()) < curr_dist) {
          curr_best = i;
          curr_dist = dataset.distance (i, route.back ());
        }
      }
      route.push_back (curr_best);
    }
    return route;
  };

  auto route = bootstrap ();

  int threshold = 1024;
  while (threshold > 0) {
    bool improved = false;

    for (int i = 1; i < route.size (); ++i) {
      for (int j = i + 2; j < route.size (); ++j) {
        if (route.flip_profit (i, j) <= -threshold) {
          route.flip (i, j);
          improved = true;
        }
      }
    }
    if (!improved)
      threshold /= 2;
  }

  return route;
}