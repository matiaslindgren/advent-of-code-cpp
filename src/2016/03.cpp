import std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Triple {
  int a;
  int b;
  int c;
};

bool is_triangle(const Triple& t) {
  const auto [a, b, c] = t;
  return a + b > c && b + c > a && c + a > b;
}

using Ints = std::vector<int>;
using Triples = std::vector<Triple>;

Triples parse_row_order(const Ints& input) {
  Triples result;
  for (std::size_t row{}; row < input.size() / 3; ++row) {
    const auto i{3 * row};
    result.push_back({input[i], input[i + 1], input[i + 2]});
  }
  return result;
}

Triples parse_col_order(const Ints& input) {
  Triples result;
  for (std::size_t col{}; col < 3; ++col) {
    for (std::size_t row{}; row < input.size() / 3; row += 3) {
      const auto i{3 * row + col};
      result.push_back({input[i], input[i + 3], input[i + 6]});
    }
  }
  return result;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto input = views::istream<int>(std::cin) | ranges::to<Ints>();
  if (input.size() % 3) {
    throw std::runtime_error("input must be divisible by 3");
  }

  const auto part1{ranges::count_if(parse_row_order(input), is_triangle)};
  const auto part2{ranges::count_if(parse_col_order(input), is_triangle)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
