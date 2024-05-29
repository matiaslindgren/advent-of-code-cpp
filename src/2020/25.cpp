#include "std.hpp"

long step(const long x, const long subject) {
  return (x * subject) % 20201227;
}

long transform(const long subject, const long loop_size) {
  long x{1};
  for (long l{1}; l <= loop_size; ++l) {
    x = step(x, subject);
  }
  return x;
}

long search_loop_size(const long subject, const long key) {
  long x{1};
  for (long l{};; ++l) {
    if (x == key) {
      return l;
    }
    x = step(x, subject);
  }
}

auto search(const int card, const int door) {
  return transform(card, search_loop_size(7, door));
}

int main() {
  if (int card{}, door{}; std::cin >> card >> door) {
    std::println("{}", search(card, door));
    return 0;
  }
  throw std::runtime_error("failed parsing input, it should be a pair of integers");
}
