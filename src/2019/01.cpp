#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

auto compute(auto masses) {
  std::vector<int> results;
  while (not masses.empty()) {
    ranges::for_each(masses, [](int& x) { x = x / 3 - 2; });
    std::erase_if(masses, [](int x) { return x <= 0; });
    results.push_back(sum(masses));
  }
  return results;
}

int main() {
  const auto masses{aoc::parse_items<int>("/dev/stdin")};
  const auto results{compute(masses)};

  const auto part1{results.front()};
  const auto part2{sum(results)};
  std::println("{} {}", part1, part2);

  return 0;
}
