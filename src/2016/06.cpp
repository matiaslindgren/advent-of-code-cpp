#include "std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using CharPairs = std::vector<std::tuple<char, char>>;

CharPairs minmax_char_freq_by_column(const auto& lines) {
  if (lines.empty()) {
    throw std::runtime_error("empty input");
  }
  const auto col_count{lines.front().size()};
  return (
      views::iota(0uz, col_count) | views::transform([&lines](const auto column) {
        std::unordered_map<char, int> char_count;
        for (const auto& line : lines) {
          ++char_count[line[column]];
        }
        const auto get_freq{[](const auto& p) { return p.second; }};
        auto [min, max]{ranges::minmax_element(char_count, {}, get_freq)};
        return std::tuple{min->first, max->first};
      })
      | ranges::to<CharPairs>()
  );
}

int main() {
  const auto lines{aoc::slurp<std::string>("/dev/stdin")};

  const CharPairs minmax_chars{minmax_char_freq_by_column(lines)};

  const auto part1{views::elements<1>(minmax_chars) | ranges::to<std::string>()};
  const auto part2{views::elements<0>(minmax_chars) | ranges::to<std::string>()};
  std::print("{} {}\n", part1, part2);

  return 0;
}
