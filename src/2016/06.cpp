#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using CharPairs = std::vector<std::pair<char, char>>;

CharPairs minmax_char_freq_by_column(const auto& lines) {
  const auto col_count{lines.front().size()};
  return views::transform(
             views::iota(0UZ, col_count),
             [&lines](std::size_t col) {
               std::unordered_map<char, int> char_count;
               for (const auto& line : lines) {
                 char_count[line.at(col)] += 1;
               }
               auto [min, max]{ranges::minmax(views::keys(char_count), {}, [&](char ch) {
                 return char_count.at(ch);
               })};
               return std::pair{min, max};
             }
         )
         | ranges::to<CharPairs>();
}

int main() {
  const auto lines{aoc::parse_items<std::string>("/dev/stdin")};
  if (lines.empty()) {
    throw std::runtime_error("input must not be empty");
  }

  const CharPairs minmax_chars{minmax_char_freq_by_column(lines)};

  const auto part1{views::elements<1>(minmax_chars) | ranges::to<std::string>()};
  const auto part2{views::elements<0>(minmax_chars) | ranges::to<std::string>()};
  std::println("{} {}", part1, part2);

  return 0;
}
