#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

int solve(const auto& digits, const auto n) {
  return sum(my_std::views::enumerate(digits) | views::transform([&](const auto& p) {
               auto&& [i, a]{p};
               const auto b{digits.at((i + n) % digits.size())};
               return (a == b) * a;
             }));
}

auto parse_digits(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  return views::istream<char>(is)
         | views::transform([](char ch) { return std::clamp(ch, '0', '9') - '0'; })
         | ranges::to<std::vector<int>>();
}

int main() {
  const auto digits{parse_digits("/dev/stdin")};
  if (digits.empty() or digits.size() % 2 != 0) {
    throw std::runtime_error("input needs to contain an even amount of digits");
  }

  const auto part1{solve(digits, 1)};
  const auto part2{solve(digits, digits.size() / 2)};

  std::println("{} {}", part1, part2);

  return 0;
}
