#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto parse_digits(std::istream& is) {
  return views::istream<char>(is)
         | views::transform([](char ch) { return std::clamp(ch, '0', '9') - '0'; })
         | ranges::to<std::vector<int>>();
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

int solve(const auto& digits, const auto n) {
  return sum(my_std::views::enumerate(digits) | views::transform([&](const auto& p) {
               const auto [i, a] = p;
               const auto b{digits[(i + n) % digits.size()]};
               return (a == b) * a;
             }));
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto digits{parse_digits(input)};
  if (digits.empty() or digits.size() % 2 != 0) {
    throw std::runtime_error("input needs to contain an even amount of digits");
  }

  const auto part1{solve(digits, 1)};
  const auto part2{solve(digits, digits.size() / 2)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
