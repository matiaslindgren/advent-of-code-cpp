#include "std.hpp"
#include "my_std.hpp"
#include "aoc.hpp"

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
      | views::filter(my_std::apply_fn(std::equal_to{}))
      | views::elements<0>
      | ranges::to<std::string>()
  );
  // clang-format on
}

auto find_part1(const auto& lines) {
  return count_repeating(lines, 2) * count_repeating(lines, 3);
}

auto find_part2(const auto& lines) {
  for (auto l1{lines.begin()}; l1 != lines.end(); ++l1) {
    for (auto l2{l1 + 1}; l2 != lines.end(); ++l2) {
      if (const auto& is{intersect(*l1, *l2)}; is.size() + 1 == l1->size()) {
        return is;
      }
    }
  }
  return std::string{};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto lines{views::istream<std::string>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(lines)};
  const auto part2{find_part2(lines)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
