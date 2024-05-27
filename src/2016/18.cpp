#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  trap = '^',
  safe = '.',
};

bool is_trap(const auto a, const auto b, const auto c) {
  // clang-format off
  return (
    (not a and not b and c)
    or (a and not b and not c)
    or (not a and b and c)
    or (a and b and not c)
  );
  // clang-format on
}

auto count_safe_tiles(const auto& tiles, const auto line_count) {
  std::bitset<102> safe{};
  if (tiles.size() + 2 > safe.size()) {
    throw std::runtime_error("too many tiles, cannot use std::bitset");
  }
  safe.set();
  for (auto&& [i, t] : my_std::views::enumerate(tiles, 1)) {
    safe[i] = (t == Tile::safe);
  }
  int n{};
  for (int l{}; l < line_count; ++l) {
    n += safe.count() - (safe.size() - tiles.size());
    const auto prev{safe};
    for (auto i{0UZ}; i < tiles.size(); ++i) {
      safe[i + 1] = not is_trap(prev[i], prev[i + 1], prev[i + 2]);
    }
  }
  return n;
}

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch{}; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::trap):
      case std::to_underlying(Tile::safe): {
        tile = {ch};
      } break;
      default:
        throw std::runtime_error(std::format("unknown tile '{}'", ch));
    }
  }
  return is;
}

int main() {
  const auto tiles{aoc::parse_items<Tile>("/dev/stdin")};

  const auto part1{count_safe_tiles(tiles, 40)};
  const auto part2{count_safe_tiles(tiles, 400'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
