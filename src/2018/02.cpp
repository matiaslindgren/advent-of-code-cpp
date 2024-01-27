import std;
import aoc;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

auto count_repeating(const auto& lines, const int r) {
  return ranges::count_if(lines, [&r](const auto& line) {
    std::array<int, 'z' - 'a' + 1> freq;
    freq.fill(0);
    for (char ch : line) {
      ++freq[ch - 'a'];
    }
    return ranges::find(freq, r) != freq.end();
  });
}

auto intersect(std::string_view s1, std::string_view s2) {
  // clang-format off
  return (
      views::zip(s1, s2)
      | views::filter([](const auto& p) { return p.first == p.second; })
      | views::transform([](const auto& p) { return p.first; })
      | ranges::to<std::string>()
  );
  // clang-format on
}

auto find_part1(const auto& lines) {
  return count_repeating(lines, 2) * count_repeating(lines, 3);
}

auto find_part2(const auto& lines) {
  for (const auto& line1 : lines) {
    for (const auto& line2 : lines) {
      if (const auto& is{intersect(line1, line2)}; is.size() == line1.size() - 1) {
        return is;
      }
    }
  }
  return ""s;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto lines{views::istream<std::string>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(lines)};
  const auto part2{find_part2(lines)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
