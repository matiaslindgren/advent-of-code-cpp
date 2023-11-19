import std;

namespace ranges = std::ranges;
namespace views = std::views;

using CharPairs = std::vector<std::tuple<char, char>>;

CharPairs minmax_char_freq_by_column(const auto& lines) {
  if (lines.empty()) {
    throw std::runtime_error("empty input");
  }
  auto&& col_count{lines.front().size()};
  // clang-format off
  return (
      views::iota(0uz, col_count)
      | views::transform([&lines](const auto column) {
          using CharCount = std::unordered_map<char, int>;
          CharCount cc;
          for (const auto& line : lines) {
            ++cc[line[column]];
          }
          auto&& get_freq{[](auto&& p) { return p.second; }};
          auto [min, max] = ranges::minmax_element(cc, {}, get_freq);
          return std::make_tuple(min->first, max->first);
      })
      | ranges::to<CharPairs>()
  );
  // clang-format on
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto lines{views::istream<std::string>(std::cin) | ranges::to<std::vector<std::string>>()};

  const CharPairs minmax_chars{minmax_char_freq_by_column(lines)};

  const auto part1{views::elements<1>(minmax_chars) | ranges::to<std::string>()};
  const auto part2{views::elements<0>(minmax_chars) | ranges::to<std::string>()};
  std::print("{} {}\n", part1, part2);

  return 0;
}
