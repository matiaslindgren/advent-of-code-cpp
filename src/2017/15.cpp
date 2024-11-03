#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

constexpr auto limit{(1U << 31) - 1U};

auto next_a(auto a) {
  return (a * 16807U) % limit;
}

auto next_b(auto b) {
  return (b * 48271U) % limit;
}

bool is_match(auto a, auto b) {
  return (a & 0xffff) == (b & 0xffff);
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

int find_part1(unsigned long a, unsigned long b) {
  return sum(views::transform(views::iota(0UL, 40'000'000UL), [&](auto i) {
    a = next_a(a);
    b = next_b(b);
    return is_match(a, b);
  }));
}

int find_part2(unsigned long a, unsigned long b) {
  return sum(views::transform(views::iota(0UL, 5'000'000UL), [&](auto i) {
    while ((a = next_a(a)) % 4 != 0U) {
    }
    while ((b = next_b(b)) % 8 != 0U) {
    }
    return is_match(a, b);
  }));
}

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int a{}; is >> skip("Generator"s, "A"s, "starts"s, "with"s) >> a) {
    if (int b{}; is >> std::ws >> skip("Generator"s, "B"s, "starts"s, "with"s) >> b) {
      return std::pair{a, b};
    }
  }
  throw std::runtime_error("failed parsing generator starting values");
}

int main() {
  const auto [a, b]{parse_input("/dev/stdin")};

  const auto part1{find_part1(a, b)};
  const auto part2{find_part2(a, b)};

  std::println("{} {}", part1, part2);

  return 0;
}
