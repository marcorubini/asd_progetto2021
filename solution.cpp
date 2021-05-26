#include <asd_progetto2021/common.hpp>

#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>

int main ()
{
  auto rng = std::mt19937 (std::random_device {}());

  auto const data = read_dataset (std::cin);
  auto route = std::vector<int> (data.num_vertices ());
  auto stones = std::vector<int> (data.num_vertices (), -1);

  std::iota (route.begin (), route.end (), 0);
  std::swap (route[0], route[data.starting_city ()]);
  std::shuffle (route.begin () + 1, route.end (), rng);
  route.push_back (data.starting_city ());

  write_output (std::cout, data, stones, route);
}