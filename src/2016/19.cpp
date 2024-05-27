#include "std.hpp"

long josephus(const auto n) {
  // https://en.wikipedia.org/wiki/Josephus_problem
  long res{};
  for (long i{1}; i <= n; ++i) {
    res = res % i + 1;
    res += res > (i + 1) / 2;
  }
  return res - 1;
}

long ipow2(int x) {
  long p{1};
  for (int i{}; i < x; ++i) {
    p *= 2;
  }
  return p;
}

int ilog2(long x) {
  int l{};
  while ((x /= 2) > 0) {
    l += 1;
  }
  return l;
}

int main() {
  std::ios::sync_with_stdio(false);
  if (int n_elves{}; std::cin >> n_elves and n_elves > 0) {
    const auto part1{1 + 2 * (n_elves - ipow2(ilog2(n_elves)))};
    const auto part2{josephus(n_elves)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input, it should be a single positive integer");
}
