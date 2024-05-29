#include "aoc.hpp"
#include "std.hpp"

auto find_frequencies(const auto& items) {
  const auto n{items.size()};
  int sum1{};
  int sum2{};
  std::unordered_map<int, int> sum_counts;
  for (auto i{0UZ}; i < 10'000'000; ++i) {
    if (i == n) {
      sum1 = sum2;
    }
    sum2 += items[i % n];
    if (++sum_counts[sum2] == 2) {
      return std::pair{sum1, sum2};
    }
  }
  throw std::runtime_error("search space exhausted, no answer");
}

int main() {
  const auto items{aoc::parse_items<int>("/dev/stdin")};
  const auto [part1, part2]{find_frequencies(items)};
  std::println("{} {}", part1, part2);
  return 0;
}
