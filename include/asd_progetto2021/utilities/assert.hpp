#pragma once
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

#if defined(EVAL) || defined(RELEASE)

#  define ASSERT(...) (void)0

#else

#  define ASSERT(...)                                                                                        \
    do {                                                                                                     \
      if (!(__VA_ARGS__)) {                                                                                  \
        fprintf (stderr, "ASSERTION FAILED: %s\n", #__VA_ARGS__);                                            \
        fprintf (stderr, "LINE: %d FILE: %s FUNCTION: %s\n", __LINE__, __FILE__, __FUNCTION__);              \
        std::terminate ();                                                                                   \
      }                                                                                                      \
    } while (0)

#endif

#if defined(EVAL) || defined(RELEASE)

#  define CHECK(...)                                                                                         \
    do {                                                                                                     \
      if (!(__VA_ARGS__))                                                                                    \
        __builtin_unreachable ();                                                                            \
    } while (0)

#else

#  define CHECK(...)                                                                                         \
    do {                                                                                                     \
      if (!(__VA_ARGS__)) {                                                                                  \
        fprintf (stderr, "CHECK FAILED: %s\n", #__VA_ARGS__);                                                \
        fprintf (stderr, "LINE: %d FILE: %s FUNCTION: %s\n", __LINE__, __FILE__, __FUNCTION__);              \
        std::terminate ();                                                                                   \
      }                                                                                                      \
    } while (0)
#endif

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Milli = std::chrono::milliseconds;
