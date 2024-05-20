#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Scanner {
  int depth{}, range{};

  bool is_at_top(const auto t) const {
    const auto n{2 * (range - 1)};
    const auto layer{std::abs(t - n * static_cast<int>(std::round(t / n)))};
    return layer == 0;
  }

  auto severity() const {
    return depth * range;
  }
};

std::istream& operator>>(std::istream& is, Scanner& scanner) {
  using aoc::skip;
  using std::operator""s;
  if (int depth, range; is >> depth and depth >= 0 and is >> skip(":"s) >> range and range > 1) {
    scanner = {depth, range};
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Scanner");
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& scanners) {
  return sum(views::transform(scanners, [](const auto& s) {
    return s.is_at_top(s.depth) * s.severity();
  }));
}

auto find_part2(const auto& scanners) {
  for (int t{};; ++t) {
    if (not ranges::any_of(scanners, [&t](const auto& s) { return s.is_at_top(t + s.depth); })) {
      return t;
    }
  }
}

int main() {
  const auto scanners{aoc::parse_items<Scanner>("/dev/stdin")};

  const auto part1{find_part1(scanners)};
  const auto part2{find_part2(scanners)};

  std::println("{} {}", part1, part2);

  return 0;
}
