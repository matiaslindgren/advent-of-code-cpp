import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

auto find_exit(auto jumps, int limit = std::numeric_limits<int>::max()) {
  int jump_count{};
  for (auto j{0uz}; 0 <= j and j < jumps.size(); ++jump_count) {
    const auto n{jumps[j]};
    jumps[j] += 1 - 2 * (n >= limit);
    j += n;
  }
  return jump_count;
}

int main() {
  const auto jumps{aoc::slurp<int>("/dev/stdin")};
  if (jumps.empty()) {
    throw std::runtime_error("cannot do anything with empty input");
  }

  const auto part1{find_exit(jumps)};
  const auto part2{find_exit(jumps, 3)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
