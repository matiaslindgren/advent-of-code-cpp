#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

bool is_triangle(const int a, const int b, const int c) {
  return a + b > c and b + c > a and c + a > b;
}

using Ints = std::vector<int>;

int count_triangles_row_order(const Ints& input) {
  int n{};
  for (std::size_t row{}; row < input.size() / 3; ++row) {
    const auto i{3 * row};
    n += is_triangle(input[i], input[i + 1], input[i + 2]);
  }
  return n;
}

int count_triangles_col_order(const Ints& input) {
  int n{};
  for (std::size_t col{}; col < 3; ++col) {
    for (std::size_t row{}; row < input.size() / 3; row += 3) {
      const auto i{3 * row + col};
      n += is_triangle(input[i], input[i + 3], input[i + 6]);
    }
  }
  return n;
}

int main() {
  const auto ints{aoc::parse_items<int>("/dev/stdin")};
  if (ints.size() % 3) {
    throw std::runtime_error("input must be divisible by 3");
  }

  const auto part1{count_triangles_row_order(ints)};
  const auto part2{count_triangles_col_order(ints)};

  std::println("{} {}", part1, part2);

  return 0;
}
