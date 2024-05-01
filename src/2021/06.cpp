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
    fish.at(t) += 1;
  }
  for (int day{}; day < days; ++day) {
    fish.at((day + 7) % 9) += fish.at(day % 9);
  }
  return sum(fish);
}

auto parse_timers(std::string path) {
  auto input{aoc::slurp_file(path)};
  ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  return views::istream<int>(is) | views::transform([](int t) {
           if (0 <= t and t < 9) {
             return t;
           }
           throw std::runtime_error("every timer must be in [0, 10)");
         })
         | ranges::to<std::vector>();
}

int main() {
  const auto timers{parse_timers("/dev/stdin")};
  const auto part1{simulate(timers, 80)};
  const auto part2{simulate(timers, 256)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
