#pragma once
#pragma GCC optimize("Ofast")
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef EVAL

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

#define CHECK(...)                                                                                           \
  do {                                                                                                       \
    if (!(__VA_ARGS__)) {                                                                                    \
      fprintf (stderr, "CHECK FAILED: %s\n", #__VA_ARGS__);                                                  \
      fprintf (stderr, "LINE: %d FILE: %s FUNCTION: %s\n", __LINE__, __FILE__, __FUNCTION__);                \
      std::terminate ();                                                                                     \
    }                                                                                                        \
  } while (0)
