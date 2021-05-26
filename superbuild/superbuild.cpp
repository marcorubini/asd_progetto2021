#include <stdexcept>
#define ASSERT(...)                                                                                          \
  do {                                                                                                       \
    if (!(__VA_ARGS__)) {                                                                                    \
      fprintf (stderr, "ASSERTION FAILED: %s\n", #__VA_ARGS__);                                              \
      fprintf (stderr, "LINE: %d FILE: %s\n", __LINE__, __FILE__);                                           \
      fprintf (stderr, "FUNCTION: %s\n", __FUNCTION__);                                                      \
      std::terminate ();                                                                                     \
    }                                                                                                        \
  } while (0)
#include <iostream>
int main ()
{
  std::cout << "Hello world!\n";
  ASSERT (false);
}
