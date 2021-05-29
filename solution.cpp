#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>

#include <asd_progetto2021/solutions/baseline.hpp>
#include <asd_progetto2021/solutions/random.hpp>
#include <asd_progetto2021/solutions/tsp_matching.hpp>
#include <asd_progetto2021/utilities/io.hpp>

int main ()
{
  std::ios_base::sync_with_stdio (false);

#ifdef EVAL
  auto is = std::ifstream ("input.txt");
  auto os = std::ofstream ("output.txt");
#else
  auto& is = std::cin;
  auto& os = std::cout;
#endif

  is.tie (0);
  os.tie (0);
  is.sync_with_stdio (false);
  os.sync_with_stdio (false);

  auto const data = read_dataset (is);
  auto const sol = solve_tsp_matching (data);

  write_output (os, sol.first, sol.second);
}