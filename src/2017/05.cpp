#include "aoc.hpp"
#include "std.hpp"

auto find_exit(auto jumps, int limit = std::numeric_limits<int>::max()) {
  int jump_count{};
  for (auto j{0UZ}; 0 <= j and j < jumps.size(); ++jump_count) {
    const auto n{jumps[j]};
    jumps[j] += 1 - 2 * (n >= limit);
    j += n;
  }
  return jump_count;
}

int main() {
  const auto jumps{aoc::parse_items<int>("/dev/stdin")};

  const auto part1{find_exit(jumps)};
  const auto part2{find_exit(jumps, 3)};

  std::println("{} {}", part1, part2);

  return 0;
}
