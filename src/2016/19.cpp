#include "std.hpp"

int josephus(const auto n) {
  int res{};
  for (int i{1}; i <= n; ++i) {
    res = res % i + 1;
    res += res > (i + 1) / 2;
  }
  return res - 1;
}

long pow2(int x) {
  auto p{1L};
  for (int i{}; i < x; ++i) {
    p *= 2;
  }
  return p;
}

int log2(long x) {
  int l{};
  while (x >>= 1) {
    ++l;
  }
  return l;
}

int main() {
  std::ios::sync_with_stdio(false);

  int elf_count;
  std::cin >> elf_count;

  const auto part1{1 + 2 * (elf_count - pow2(log2(elf_count)))};
  const auto part2{josephus(elf_count)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
