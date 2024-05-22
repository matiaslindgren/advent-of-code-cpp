#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;

auto deliver_presents(
    const int target,
    const int delivery_count,
    const int house_count = std::numeric_limits<int>::max()
) {
  std::vector<long> presents(target);
  for (long elf{1}; delivery_count * elf < target; ++elf) {
    for (long house{1}; house <= house_count and delivery_count * house * elf < target; ++house) {
      presents.at(house * elf) += delivery_count * elf;
    }
  }
  const auto house_match{ranges::find_if(presents, [&target](auto p) { return p >= target; })};
  return ranges::distance(presents.begin(), house_match);
}

int main() {
  if (int target{}; std::cin >> target) {
    const auto part1{deliver_presents(target, 10)};
    const auto part2{deliver_presents(target, 11, 50)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input, should be a single integer");
}
