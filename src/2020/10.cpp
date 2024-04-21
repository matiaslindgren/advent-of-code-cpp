#include "std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto count_arrangements(const auto& jolts) {
  std::unordered_map<int, long> counts{{0, 1}};
  for (int jolt : views::drop(jolts, 1)) {
    for (int i{1}; i <= 3; ++i) {
      counts[jolt] += counts[jolt - i];
    }
  }
  return counts[jolts.back()];
}

auto search(auto jolts) {
  ranges::sort(jolts);

  jolts.insert(jolts.begin(), 0);
  jolts.push_back(jolts.back() + 3);

  std::unordered_map<int, int> diffs;
  // TODO views::pairwise
  for (const auto [j1, j2] : views::zip(jolts, views::drop(jolts, 1))) {
    diffs[j2 - j1] += 1;
  }

  return std::pair{diffs[1] * diffs[3], count_arrangements(jolts)};
}

int main() {
  const auto [part1, part2]{search(aoc::slurp<int>("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
