#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto compute(auto masses) {
  std::vector<int> results;
  for (; ranges::any_of(masses, [](int x) { return x > 0; });) {
    ranges::for_each(masses, [](int& x) { x = std::max(0, x / 3 - 2); });
    results.push_back(sum(masses));
  }
  return results;
}

int main() {
  const auto masses{aoc::slurp<int>("/dev/stdin")};
  const auto results{compute(masses)};

  const auto part1{results.front()};
  const auto part2{sum(results)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
