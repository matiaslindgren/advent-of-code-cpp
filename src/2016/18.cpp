import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  trap = '^',
  safe = '.',
};

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::trap):
      case std::to_underlying(Tile::safe):
        tile = {ch};
        return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Tile");
}

bool is_trap(const auto a, const auto b, const auto c) {
  return (!a && !b && c) || (a && !b && !c) || (!a && b && c) || (a && b && !c);
}

auto count_safe_tiles(const auto& tiles, const auto line_count) {
  std::bitset<102> safe{};
  if (tiles.size() + 2 > safe.size()) {
    throw std::runtime_error("too many tiles, cannot use std::bitset");
  }
  safe.set();
  for (const auto& [i, t] : my_std::views::enumerate(tiles, 1)) {
    safe[i] = (t == Tile::safe);
  }
  int n{};
  for (int l{}; l < line_count; ++l) {
    n += safe.count() - (safe.size() - tiles.size());
    const auto prev{safe};
    for (auto i{0uz}; i < tiles.size(); ++i) {
      safe[i + 1] = !is_trap(prev[i], prev[i + 1], prev[i + 2]);
    }
  }
  return n;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto tiles{views::istream<Tile>(input) | ranges::to<std::vector>()};

  const auto part1{count_safe_tiles(tiles, 40)};
  const auto part2{count_safe_tiles(tiles, 400000)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
