#pragma once
#include <array>

template<unsigned Capacity>
struct FenwickTree
{
private:
  std::array<int, Capacity> _sum {};
  int _size {};

public:
  FenwickTree () = default;

  auto size () const -> int
  {
    return _size;
  }

  auto prefix_sum (int last) const -> int
  {
    int sum = 0;
    while (last >= 0) {
      sum += _sum[last];
      last = (last & (last + 1)) - 1;
    }
    return sum;
  }

  auto sum (int first, int last) const -> int
  {
    return prefix_sum (last) - prefix_sum (first - 1);
  }

  auto add (int index, int value) -> void
  {
    while (index < Capacity) {
      _sum[index] += value;
      index = index | (index + 1);
    }
  }
};
