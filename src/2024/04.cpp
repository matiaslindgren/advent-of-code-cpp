#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Grid {
  std::unordered_map<Vec2, char> tiles;

  [[nodiscard]]
  char get(const Vec2& p) const {
    return tiles.contains(p) ? tiles.at(p) : char{};
  }
};

auto find_part1(const Grid& grid) {
  static constexpr std::array directions{
      std::array{Vec2(1, 0), Vec2(2, 0), Vec2(3, 0)},
      std::array{Vec2(-1, 0), Vec2(-2, 0), Vec2(-3, 0)},
      std::array{Vec2(1, 1), Vec2(2, 2), Vec2(3, 3)},
      std::array{Vec2(-1, -1), Vec2(-2, -2), Vec2(-3, -3)},
      std::array{Vec2(0, 1), Vec2(0, 2), Vec2(0, 3)},
      std::array{Vec2(0, -1), Vec2(0, -2), Vec2(0, -3)},
      std::array{Vec2(-1, 1), Vec2(-2, 2), Vec2(-3, 3)},
      std::array{Vec2(1, -1), Vec2(2, -2), Vec2(3, -3)},
  };
  int n{};
  for (const auto& [pos, tile] : grid.tiles) {
    if (tile == 'X') {
      n += ranges::count_if(directions, [&](std::array<Vec2, 3> v) {
        return grid.get(pos + v[0]) == 'M' and grid.get(pos + v[1]) == 'A'
               and grid.get(pos + v[2]) == 'S';
      });
    }
  }
  return n;
}

auto find_part2(const Grid& grid) {
  int n{};
  for (const auto& [pos, tile] : grid.tiles) {
    const auto check{[&](std::string s) {
      return grid.get(pos + Vec2(-1, -1)) == s[0] and grid.get(pos + Vec2(1, -1)) == s[1]
             and grid.get(pos + Vec2(1, 1)) == s[2] and grid.get(pos + Vec2(-1, 1)) == s[3];
    }};
    if (tile == 'A' and (check("MMSS") or check("SMMS") or check("SSMM") or check("MSSM"))) {
      n += 1;
    }
  }

  return n;
}

auto parse_grid(std::string_view path) {
  Grid g{};
  std::size_t width{};
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (width == 0U) {
      width = line.size();
    } else if (width != line.size()) {
      throw std::runtime_error("every row must be of equal length");
    }
    for (p.x() = 0; char ch : line) {
      switch (ch) {
        case 'X':
        case 'M':
        case 'A':
        case 'S': {
          g.tiles[p] = {ch};
        } break;
        default: {
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
        }
      }
      p.x() += 1;
    }
  }
  return g;
}

int main() {
  Grid grid{parse_grid("/dev/stdin")};

  const auto part1{find_part1(grid)};
  const auto part2{find_part2(grid)};

  std::println("{} {}", part1, part2);

  return 0;
}
