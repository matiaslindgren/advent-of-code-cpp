#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

long pow(int x, int p) {
  long res{1};
  while (p-- > 0) {
    res *= x;
  }
  return res;
}

long parse_snafu(std::string_view line) {
  return sum(views::transform(my_std::views::enumerate(line), [n = line.size()](auto item) {
    auto [i, ch]{item};
    auto x{pow(5, n - (i + 1))};
    switch (ch) {
      case '0': {
        x = 0;
      } break;
      case '-': {
        x = -x;
      } break;
      case '=': {
        x *= -2;
      } break;
      case '2': {
        x *= 2;
      } break;
    }
    return x;
  }));
}

auto find_part1(const auto& lines) {
  std::vector<int> snafu_digits;
  {
    std::vector<int> base5_digits;
    for (auto x{sum(views::transform(lines, parse_snafu))}; x > 0;) {
      auto [q, r]{std::ldiv(x, 5)};
      base5_digits.push_back(r);
      x = q;
    }
    int carry{};
    for (int d5 : base5_digits) {
      d5 += carry;
      carry = d5 / 3;
      snafu_digits.push_back(d5 - 5 * carry);
    }
    if (carry) {
      snafu_digits.push_back(carry);
    }
  }
  return ranges::fold_left(
      snafu_digits | views::reverse,
      std::string{},
      [](std::string res, int ds) {
        switch (ds) {
          case 0:
          case 1:
          case 2: {
            res.push_back('0' + ds);
          } break;
          case -1: {
            res.push_back('-');
          } break;
          case -2: {
            res.push_back('=');
          } break;
          default: {
            throw std::runtime_error(std::format("invalid snafu digit {}", ds));
          }
        }
        return res;
      }
  );
}

int main() {
  const auto lines{aoc::slurp_lines("/dev/stdin")};
  const auto part1{find_part1(lines)};
  std::println("{}", part1);
  return 0;
}
