#include "std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto get_digits(auto x) {
  std::vector<int> d;
  while (x) {
    const auto [q, r]{std::div(x, 10)};
    x = q;
    d.push_back(r);
  }
  return d;
}

// TODO ranges::adjacent
constexpr decltype(auto) window2(ranges::random_access_range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

auto search(const auto begin, const auto end) {
  int part1{}, part2{};

  for (auto x{begin}; x <= end; ++x) {
    const auto digits{get_digits(x)};

    int repeats{1};
    bool has_two{false};
    bool has_two_once{false};
    bool is_monotonic{true};

    for (auto&& [d1, d2] : window2(digits)) {
      if (d1 == d2) {
        has_two = true;
        repeats += 1;
      } else {
        has_two_once |= repeats == 2;
        repeats = 1;
      }
      is_monotonic &= d2 <= d1;
    }

    has_two_once |= repeats == 2;

    part1 += has_two and is_monotonic;
    part2 += has_two_once and is_monotonic;
  }

  return std::pair{part1, part2};
}

auto parse_input(const std::string path) {
  using aoc::skip;
  using std::operator""s;

  std::istringstream input{aoc::slurp_file(path)};
  if (unsigned begin, end; input >> begin >> skip("-"s) >> end >> std::ws) {
    if (input.eof()) {
      return std::pair{begin, end};
    }
  }

  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto [begin, end]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(begin, end)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
