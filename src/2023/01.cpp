#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

void replace(std::string& s, std::string_view pat, std::string_view sub) {
  for (std::string::size_type i{}; (i = s.find(pat, i)) != std::string::npos;) {
    s.erase(i, pat.size());
    s.insert(i, sub);
    i += sub.size();
  }
}

using std::operator""sv;

constexpr std::array number_names{
    "one"sv,
    "two"sv,
    "three"sv,
    "four"sv,
    "five"sv,
    "six"sv,
    "seven"sv,
    "eight"sv,
    "nine"sv,
};

auto prepare_part2(const auto& lines) {
  return views::transform(
             lines,
             [&](std::string line) {
               for (auto&& [number, name] : my_std::views::enumerate(number_names, 1)) {
                 replace(line, name, std::format("{}{}{}", name, number, name));
               }
               return line;
             }
         )
         | ranges::to<std::vector>();
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto calibrate(const auto& lines) {
  return sum(views::transform(lines, [&](std::string_view line) {
    auto nums{views::filter(line, aoc::is_digit) | ranges::to<std::vector>()};
    if (nums.empty()) {
      throw std::runtime_error("line should have at least one digit after calibration");
    }
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
