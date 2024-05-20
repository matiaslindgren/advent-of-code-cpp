#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto parse_calories(std::string path) {
  std::vector<long> calories;
  {
    auto lines{aoc::slurp_lines(path)};
    lines.emplace_back();
    long current_sum{};
    for (auto line : lines) {
      if (line.empty()) {
        calories.push_back(std::exchange(current_sum, 0));
      } else if (auto c{std::atoi(line.c_str())}) {
        current_sum += c;
      } else {
        throw std::runtime_error(
            std::format("all non-whitespace input must be digits, not '{}'", line)
        );
      }
    }
  }
  if (calories.empty()) {
    throw std::runtime_error("empty input");
  }
  return calories;
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

int main() {
  auto calories{parse_calories("/dev/stdin")};
  ranges::sort(calories, ranges::greater{});

  const auto part1{calories.front()};
  const auto part2{sum(calories | views::take(3))};

  std::println("{} {}", part1, part2);

  return 0;
}
