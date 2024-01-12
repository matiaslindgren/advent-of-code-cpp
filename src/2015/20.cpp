import std;
import aoc;

namespace ranges = std::ranges;

int deliver_presents(
    const int target,
    const int delivery_count,
    const int house_count = std::numeric_limits<int>::max()
) {
  std::vector<int> presents(target);
  for (int elf{1}; delivery_count * elf < target; ++elf) {
    for (int house{1}; house <= house_count && delivery_count * house * elf < target; ++house) {
      presents[house * elf] += delivery_count * elf;
    }
  }
  const auto house_match{ranges::find_if(presents, [&target](auto p) { return p >= target; })};
  return ranges::distance(presents.begin(), house_match);
}

int main() {
  int target;
  std::cin >> target;

  const auto part1{deliver_presents(target, 10)};
  const auto part2{deliver_presents(target, 11, 50)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
