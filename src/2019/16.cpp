#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto take_prefix(const auto& v, std::size_t n) {
  return ranges::fold_left(
      v | views::take(n) | views::reverse,
      0UZ,
      [tens = 1L](auto res, int x) mutable { return res + x * (tens *= 10) / 10; }
  );
}

auto find_part1(auto v) {
  const std::array pattern{0, 1, 0, -1};
  for (int phase{}; phase < 100; ++phase) {
    for (auto i{0UZ}; i < v.size(); ++i) {
      long out{};
      for (auto j{0UZ}; j < v.size(); ++j) {
        const auto k{((j + 1) / (i + 1)) % pattern.size()};
        out += v[j] * pattern[k];
      }
      v[i] = std::abs(out) % 10;
    }
  }
  return take_prefix(v, 8);
}

auto find_part2(const auto& input) {
  std::vector<int> v;
  {
    const auto n{input.size()};
    const auto offset{take_prefix(input, 7)};
    v.append_range(views::drop(input, offset % n));
    for (int rep{}; rep < (10'000 * n - offset) / n; ++rep) {
      v.append_range(input);
    }
  }
  for (int phase{}; phase < 100; ++phase) {
    long out{};
    for (int& x : views::reverse(v)) {
      x = (out += x) % 10;
    }
  }
  return take_prefix(v, 8);
}

auto parse_input(const std::string path) {
  std::istringstream input{aoc::slurp_file(path)};
  auto signal{
      views::istream<char>(input) | views::transform([](char ch) -> int {
        switch (ch) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            return ch - '0';
        }
        throw std::runtime_error("input should contain only digits");
      })
      | ranges::to<std::vector>()
  };
  if (input.eof()) {
    return signal;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto input{parse_input("/dev/stdin")};

  const auto part1{find_part1(input)};
  const auto part2{find_part2(input)};

  std::println("{} {}", part1, part2);

  return 0;
}
