#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

auto search(const auto& v) {
  long part1{};
  for (auto [x, y, z] : my_std::views::cartesian_product(v, v, v)) {
    if (x + y == 2020) {
      part1 = x * y;
    }
    if (part1 and x + y + z == 2020) {
      return std::pair{part1, x * y * z};
    }
  }
  throw std::runtime_error("could not find answer");
}

int main() {
  const auto [part1, part2]{search(aoc::slurp<int>("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
