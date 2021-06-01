#pragma once
#include <ctime>

struct Timer
{
private:
  std::clock_t time_start = std::clock ();

public:
  Timer () = default;

  auto elapsed_ms () const -> double
  {
    return std::difftime (std::clock (), time_start) * 1000.0 / CLOCKS_PER_SEC;
  }
};