#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Hex {
  int q{};
  int r{};

  Hex operator+(const Hex& rhs) const {
    return {q + rhs.q, r + rhs.r};
  }

  [[nodiscard]]
  auto distance(const Hex& rhs) const {
    // https://www.redblobgames.com/grids/hexagons/#distances
    auto dq{std::abs(q - rhs.q)};
    auto ds{std::abs((q + r) - (rhs.q + rhs.r))};
    auto dr{std::abs(r - rhs.r)};
    return (dq + ds + dr) / 2;
  }
};

auto measure_distances(const auto& steps) {
  const Hex begin{0, 0};
  Hex pos{begin};
  int max_dist{};
  for (const Hex& step : steps) {
    pos = pos + step;
    max_dist = std::max(max_dist, begin.distance(pos));
  }
  return std::pair{begin.distance(pos), max_dist};
}

std::istream& operator>>(std::istream& is, Hex& h) {
  if (char c1{}; is >> c1 and (c1 == 'n' or c1 == 's')) {
    if (char c2{}; (is.peek() == 'w' or is.peek() == 'e') and is >> c2) {
      if (c1 == 'n' and c2 == 'w') {
        h = {.q = -1, .r = 0};
      } else if (c1 == 'n' and c2 == 'e') {
        h = {.q = 1, .r = -1};
      } else if (c1 == 's' and c2 == 'w') {
        h = {.q = -1, .r = 1};
      } else if (c1 == 's' and c2 == 'e') {
        h = {.q = 1, .r = 0};
      } else {
        throw std::runtime_error("failed parsing hex pair");
      }
    } else {
      h = {.q = 0, .r = c1 == 'n' ? -1 : 1};
    }
  }
  return is;
}

int main() {
  const auto steps{aoc::parse_items<Hex>("/dev/stdin", ',')};
  const auto [part1, part2]{measure_distances(steps)};
  std::println("{} {}", part1, part2);
  return 0;
}
