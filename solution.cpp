#include <algorithm>
#include <chrono>
#include <fstream>
#include <numeric>
#include <random>

#include <asd_progetto2021/dataset/io.hpp>
#include <asd_progetto2021/solutions/general.hpp>
#include <asd_progetto2021/solutions/no_tour.hpp>
#include <asd_progetto2021/solutions/selection_only.hpp>
#include <asd_progetto2021/solutions/single_matching.hpp>
#include <asd_progetto2021/solutions/tsp_only.hpp>

int main ()
{
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::steady_clock;

  auto rng = std::mt19937 (std::random_device {}());
  auto const timer = Timer ();

  std::ios_base::sync_with_stdio (false);
  std::cin.tie (0);
  std::cout.tie (0);

#ifdef EVAL
  auto is = fopen ("input.txt", "r");
  auto os = fopen ("output.txt", "w");
#else
  auto is = stdin;
  auto os = stdout;
#endif

  auto const data = read_dataset (is);

  auto const tour_does_not_matter = [&] () {
    if (data.glove_resistance () == 0.0)
      return true;

    for (int i = 0; i < data.num_cities (); ++i)
      for (int j = 0; j < i; ++j)
        if (data.distance (i, j) != data.distance (0, 1))
          return false;

    return true;
  }();

  auto const stones_dont_matter = [&] () {
    if (data.glove_capacity () == 0)
      return true;

    for (int i = 0; i < data.num_stones (); ++i)
      if (data.cities_with_stone (i).size () > 0)
        return false;

    return true;
  }();

  auto const only_one_matching = [&] () {
    for (int i = 0; i < data.num_stones (); ++i)
      if (data.cities_with_stone (i).size () > 1)
        return false;

    return true;
  }();

  // =============

  if (tour_does_not_matter && only_one_matching) {
    auto selected = solve_selection_only (data, rng);
    auto route = SimpleRoute (data);
    auto matching = StoneMatching (data);
    for (auto i : selected)
      matching.match (i, data.cities_with_stone (i).at (0));
    write_output (os, route, matching);
    return 0;
  }

  if (stones_dont_matter) {
    // find a good tour
    auto tour = solve_tsp_only (data, rng, 4900.0 - timer.elapsed_ms ());
    auto matching = StoneMatching (data);
    write_output (os, tour, matching);
    return 0;
  }

  if (tour_does_not_matter) {
    // find a complete matching and selection
    auto matching = solve_no_tour (data, rng, 4900.0 - timer.elapsed_ms ());
    auto route = SimpleRoute (data);
    write_output (os, route, matching);
    return 0;
  }

  if (only_one_matching) {
    // find a good selection and tour
    auto sol = solve_single_matching (data, rng, 4900.0 - timer.elapsed_ms ());
    write_output (os, sol.first, sol.second);
    return 0;
  }

  auto sol = solve_general (data, rng, 4900.0 - timer.elapsed_ms ());
  write_output (os, sol.first, sol.second);
}