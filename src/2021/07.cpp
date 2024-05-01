#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto search(const auto& crabs) {
  auto intmax{std::numeric_limits<int>::max()};
  return ranges::fold_left(
      views::iota(ranges::min(crabs), ranges::max(crabs) + 1),
      std::pair{intmax, intmax},
      [&crabs](auto acc, int pos) {
        auto deltas{views::transform(crabs, [&pos](int crab) { return std::abs(crab - pos); })};
        auto a{sum(deltas)};
        auto b{sum(views::transform(deltas, [](int d) { return d * (d + 1) / 2; }))};
        return std::pair{
            std::min(acc.first, a),
            std::min(acc.second, b),
        };
      }
  );
}

auto parse_crabs(std::string path) {
  auto input{aoc::slurp_file(path)};
  ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  return views::istream<int>(is) | ranges::to<std::vector>();
}

int main() {
  const auto crabs{parse_crabs("/dev/stdin")};
  const auto [part1, part2]{search(crabs)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
