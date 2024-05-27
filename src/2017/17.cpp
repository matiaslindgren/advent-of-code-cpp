#include "std.hpp"

auto find_part1(const auto steps) {
  std::vector<unsigned long> buf{0};
  long i{};
  while (buf.size() < 2018) {
    i = 1 + (i + steps) % buf.size();
    buf.insert(buf.begin() + i, buf.size());
  }
  return buf.at(i + 1);
}

auto find_part2(const auto steps) {
  auto value{0UL};
  for (auto i{0UL}, n{1UL}; n < 50'000'000; ++n) {
    i = 1 + (i + steps) % n;
    if (i == 1) {
      value = n;
    }
  }
  return value;
}

int main() {
  std::ios::sync_with_stdio(false);
  if (int steps{}; std::cin >> steps and steps > 0) {
    const auto part1{find_part1(steps)};
    const auto part2{find_part2(steps)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input, it should be a single positive integer");
}
