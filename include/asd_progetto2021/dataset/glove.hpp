#pragma once
#include <asd_progetto2021/dataset/limits.hpp>
#include <asd_progetto2021/utilities/assert.hpp>

struct Glove
{
  int capacity {-1};
  double resistance {-1};

  Glove () = default;

  Glove (int capacity, double resistance) : capacity (capacity), resistance (resistance)
  {
    ASSERT (capacity >= 0 && capacity <= MAX_GLOVE_CAPACITY);
    ASSERT (resistance >= 0 && resistance <= MAX_GLOVE_RESISTANCE);
  }
};
