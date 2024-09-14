
#include "main.hpp"

auto main() -> int
{
  auto const lib = library {};
  auto const message = "Hello from " + lib._name + "!";
  std::cout << message << '\n';
  return 0;
}
