#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>

#include <asd_progetto2021/baseline.hpp>
#include <asd_progetto2021/dataset_io.hpp>

int main ()
{
#ifdef EVAL
  auto is = std::ifstream ("input.txt");
  auto os = std::ofstream ("output.txt");
#else
  auto& is = std::cin;
  auto& os = std::cout;
#endif

  auto rng = std::mt19937 (std::random_device {}());

  auto const data = read_dataset (is);
  auto const sol = baseline_greedy (data);

  write_output (os, sol.first, sol.second);
}