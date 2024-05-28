#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  open = '.',
  tree = '|',
  yard = '#',
};

using TileMap = std::map<Vec2, Tile>;

struct Grid {
  TileMap tiles;

  [[nodiscard]]
  auto count(Tile t) const {
    return ranges::count(tiles | views::values, t);
  }

  [[nodiscard]]
  int count_adjacent(const Vec2& center, Tile t) const {
    int n{};
    for (Vec2 d(-1, -1); d.x() <= 1; d.x() += 1) {
      for (d.y() = -1; d.y() <= 1; d.y() += 1) {
        if (Vec2 p{center + d}; p != center and tiles.contains(p) and tiles.at(p) == t) {
          n += 1;
        }
      }
    }
    return n;
  }

  [[nodiscard]]
  Grid step() const {
    Grid after{*this};
    for (auto&& [p, tile] : tiles) {
      const auto n_trees{count_adjacent(p, Tile::tree)};
      const auto n_yards{count_adjacent(p, Tile::yard)};
      switch (tile) {
        case Tile::open: {
          if (n_trees >= 3) {
            after.tiles[p] = Tile::tree;
          }
        } break;
        case Tile::tree: {
          if (n_yards >= 3) {
            after.tiles[p] = Tile::yard;
          }
        } break;
        case Tile::yard: {
          if (n_yards > 0 and n_trees > 0) {
            after.tiles[p] = Tile::yard;
          } else {
            after.tiles[p] = Tile::open;
          }
        } break;
      }
    }
    return after;
  }
};

auto simulate(Grid grid, const auto t_limit) {
  std::vector<TileMap> seen;
  for (int t{}; t < t_limit; ++t) {
    if (const auto prev{ranges::find(seen, grid.tiles)}; prev != seen.end()) {
      const auto cycle_len{ranges::distance(prev, seen.end())};
      grid.tiles = prev[(t_limit - t) % cycle_len];
      break;
    }
    seen.push_back(grid.tiles);
    grid = grid.step();
  }
  return grid.count(Tile::tree) * grid.count(Tile::yard);
}

Grid parse_grid(std::string_view path) {
  Grid g;
  for (Vec2 p; const std::string& line : aoc::slurp_lines(path)) {
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::open):
        case std::to_underlying(Tile::tree):
        case std::to_underlying(Tile::yard): {
          g.tiles[p] = {ch};
        } break;
        default:
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
    p.y() += 1;
  }
  return g;
}

int main() {
  const auto grid{parse_grid("/dev/stdin")};

  const auto part1{simulate(grid, 10)};
  const auto part2{simulate(grid, 1'000'000'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
