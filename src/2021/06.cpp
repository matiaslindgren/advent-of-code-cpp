#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0LL, std::plus{})};

auto simulate(const auto& timers, const int days) {
  // https://www.reddit.com/r/adventofcode/comments/r9z49j/comment/hng2l63
  // accessed 2024-05-01
  std::vector<long long> fish(9);
  for (auto t : timers) {
    if (0 <= t and t < fish.size()) {
      fish[t] += 1;
    } else {
      throw std::runtime_error("every timer must be in [0, 10)");
    }
  }
  for (int day{}; day < days; ++day) {
    fish[(day + 7) % fish.size()] += fish[day % fish.size()];
  }
  return sum(fish);
}

int main() {
  const auto timers{aoc::slurp<int>("/dev/stdin", ',')};
  const auto part1{simulate(timers, 80)};
  const auto part2{simulate(timers, 256)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
