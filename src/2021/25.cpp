#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  empty = '.',
  east = '>',
  south = 'v',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  int width{};
  int height{};

  [[nodiscard]]
  Grid step(Tile turn) const {
    Grid next{*this};
    for (auto [src, tile] : tiles) {
      if (tile == turn) {
        Vec2 dst{src};
        switch (tile) {
          case Tile::empty: {
            throw std::runtime_error("cannot move an empty slot");
          } break;
          case Tile::east: {
            dst.x() = (dst.x() + 1) % width;
          } break;
          case Tile::south: {
            dst.y() = (dst.y() + 1) % height;
          } break;
        }
        if (tiles.at(dst) == Tile::empty) {
          next.tiles[dst] = tiles.at(src);
          next.tiles[src] = Tile::empty;
        }
      }
    }
    return next;
  }

  bool operator==(const Grid& rhs) const {
    return tiles == rhs.tiles;
  }
};

int simulate(Grid grid) {
  for (int step{1};; ++step) {
    Grid next{grid.step(Tile::east).step(Tile::south)};
    if (next == grid) {
      return step;
    }
    grid = next;
  }
}

Grid parse_grid(std::string_view path) {
  Grid g;
  for (Vec2 p; const std::string& line : aoc::slurp_lines(path)) {
    for (p.x() = 0; char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::empty):
        case std::to_underlying(Tile::east):
        case std::to_underlying(Tile::south): {
          g.tiles[p] = {ch};
        } break;
        default:
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
    if (g.width == 0) {
      g.width = p.x();
    } else if (g.width != p.x()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.y() += 1;
    g.height += 1;
  }
  return g;
}

int main() {
  auto grid{parse_grid("/dev/stdin")};
  const auto part1{simulate(grid)};
  std::print("{}\n", part1);
  return 0;
}
