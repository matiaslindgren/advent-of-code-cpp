#include "std.hpp"

int find_part1(const auto steps) {
  std::vector<int> buf{0};
  auto i{0uz};
  while (buf.size() < 2018) {
    i = 1 + (i + steps) % buf.size();
    buf.insert(buf.begin() + i, buf.size());
  }
  return buf.at(i + 1);
}

int find_part2(const auto steps) {
  auto value{0uz};
  for (auto i{0uz}, n{1uz}; n < 50'000'000; ++n) {
    i = 1 + (i + steps) % n;
    if (i == 1) {
      value = n;
    }
  }
  return value;
}

int main() {
  std::ios::sync_with_stdio(false);

  int steps;
  std::cin >> steps;

  const auto part1{find_part1(steps)};
  const auto part2{find_part2(steps)};

  std::println("{} {}", part1, part2);

  return 0;
}
