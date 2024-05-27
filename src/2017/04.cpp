#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::vector<std::string>;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

bool is_valid(const Strings& passphrases) {
  std::unordered_map<std::string, int> freq;
  for (const auto& pp : passphrases) {
    freq[pp] += 1;
  }
  return ranges::all_of(freq | views::values, [](int f) { return f == 1; });
}

auto find_part1(const auto& rows) {
  return sum(views::transform(rows, is_valid));
}

auto find_part2(const auto& rows) {
  return sum(views::transform(rows, [](Strings row) {
    ranges::for_each(row, [](auto& pp) { ranges::sort(pp); });
    return is_valid(row);
  }));
}

auto parse_rows(std::string_view path) {
  return views::transform(
             aoc::slurp_lines(path),
             [](const std::string& line) {
               std::istringstream ls{line};
               auto row{views::istream<std::string>(ls) | ranges::to<std::vector>()};
               if (ls.eof()) {
                 return row;
               }
               throw std::runtime_error(std::format("failed parsing strings from line '{}'", line));
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
