#pragma
#include <asd_progetto2021/dataset.hpp>

#include <random>

inline auto opt3_swap (Route& route, int a, int b, int c)
{
  auto const& dataset = route.dataset ();
  ASSERT (a > 0 && a < route.size ());
  ASSERT (b >= 0 && b < route.size ());
  ASSERT (c >= 0 && c < route.size ());
  ASSERT (a < b && a < c);
  ASSERT (b < c);
}

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

    route.push_back (dataset.starting_city ());
  };
}