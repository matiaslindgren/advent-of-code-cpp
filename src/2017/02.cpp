#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& rows) {
  return sum(views::transform(rows, [](const auto& row) {
    auto [min, max]{ranges::minmax(row)};
    return max - min;
  }));
}

auto find_part2(const auto& rows) {
  return sum(views::transform(rows, [](const auto& row) {
    for (auto&& [a, b] : my_std::views::cartesian_product(row, row)) {
      if (a != b and b and a % b == 0) {
        return a / b;
      }
    }
    throw std::runtime_error("no pair is evenly divisible");
  }));
}

auto parse_rows(std::string_view path) {
  return views::transform(
             aoc::slurp_lines(path),
             [](const std::string& line) {
               std::istringstream ls{line};
               auto row{views::istream<int>(ls) | ranges::to<std::vector>()};
               if (ls.eof()) {
                 return row;
               }
               throw std::runtime_error(std::format("failed parsing integers from line '{}'", line)
               );
             }
         )
         | ranges::to<std::vector>();
}

int main() {
  const auto rows{parse_rows("/dev/stdin")};

  const auto part1{find_part1(rows)};
  const auto part2{find_part2(rows)};

  std::println("{} {}", part1, part2);

  return 0;
}
