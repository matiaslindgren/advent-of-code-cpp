#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

void replace(std::string& s, std::string_view pat, std::string_view sub) {
  for (std::string::size_type i{}; i != std::string::npos;) {
    i = s.find(pat, i);
    if (i != std::string::npos) {
      s.erase(i, pat.size());
      s.insert(i, sub);
      i += sub.size();
    }
  }
}

auto prepare_part2(const auto& lines) {
  return views::transform(
             lines,
             [&](std::string line) {
               replace(line, "one", "one1one");
               replace(line, "two", "two2two");
               replace(line, "three", "three3three");
               replace(line, "four", "four4four");
               replace(line, "five", "five5five");
               replace(line, "six", "six6six");
               replace(line, "seven", "seven7seven");
               replace(line, "eight", "eight8eight");
               replace(line, "nine", "nine9nine");
               return line;
             }
         )
         | ranges::to<std::vector>();
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto calibrate(const auto& lines) {
  return sum(views::transform(lines, [&](std::string_view line) {
    auto nums{views::filter(line, aoc::is_digit) | ranges::to<std::vector>()};
    return 10 * (nums.front() - '0') + (nums.back() - '0');
  }));
}

int main() {
  const auto lines{aoc::slurp_lines("/dev/stdin")};

  const auto part1{calibrate(lines)};
  const auto part2{calibrate(prepare_part2(lines))};

  std::println("{} {}", part1, part2);

  return 0;
}
