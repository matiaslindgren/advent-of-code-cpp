import std;
import aoc;

auto find_frequencies(const auto& items) {
  const auto n{items.size()};
  int sum1{}, sum2{};
  std::unordered_map<int, int> sum_counts;
  for (auto i{0uz};; ++i) {
    if (i == n) {
      sum1 = sum2;
    }
    sum2 += items[i % n];
    if (++sum_counts[sum2] == 2) {
      return std::pair{sum1, sum2};
    }
  }
}

int main() {
  const auto items{aoc::slurp<int>("/dev/stdin")};
  const auto [part1, part2]{find_frequencies(items)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
