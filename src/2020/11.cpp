#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  floor = '.',
  empty = 'L',
  taken = '#',
};

using Vec2 = aoc::Vec2<int>;

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  std::size_t width{};

  auto count(Tile t) const {
    return ranges::count(tiles | views::values, t);
  }

  auto get(const Vec2& p) const {
    return tiles.contains(p) ? tiles.at(p) : Tile::floor;
  }

  auto count_adjacent(Vec2 p, Tile t) const {
    int n{};
    for (Vec2 d(-1, -1); d.y() <= 1; d.y() += 1) {
      for (d.x() = -1; d.x() <= 1; d.x() += 1) {
        if (d == Vec2()) {
          continue;
        }
        n += (get(p + d) == t);
      }
    }
    return n;
  }

  auto count_queen_path(Vec2 p, Tile t) const {
    int n{};
    for (Vec2 d(-1, -1); d.y() <= 1; d.y() += 1) {
      for (d.x() = -1; d.x() <= 1; d.x() += 1) {
        if (d == Vec2()) {
          continue;
        }
        for (Vec2 p2{p + d}; tiles.contains(p2); p2 += d) {
          if (get(p2) == Tile::floor) {
            continue;
          }
          n += (get(p2) == t);
          break;
        }
      }
    }
    return n;
  }

  bool operator==(const Grid&) const = default;
};

bool step_part1(Grid& g) {
  Grid init_g{g};
  auto tiles{g.tiles};
  for (Vec2 p; p.y() < g.tiles.size() / g.width; p.y() += 1) {
    for (p.x() = 0; p.x() < g.width; p.x() += 1) {
      switch (g.tiles.at(p)) {
        case Tile::floor: {
        } break;
        case Tile::empty: {
          if (g.count_adjacent(p, Tile::taken) == 0) {
            tiles[p] = Tile::taken;
          }
        } break;
        case Tile::taken: {
          if (g.count_adjacent(p, Tile::taken) > 3) {
            tiles[p] = Tile::empty;
          }
        } break;
      }
    }
  }
  g.tiles = tiles;
  return g != init_g;
}

bool step_part2(Grid& g) {
  Grid init_g{g};
  auto tiles{g.tiles};
  for (Vec2 p; p.y() < g.tiles.size() / g.width; p.y() += 1) {
    for (p.x() = 0; p.x() < g.width; p.x() += 1) {
      switch (g.tiles.at(p)) {
        case Tile::floor: {
        } break;
        case Tile::empty: {
          if (g.count_queen_path(p, Tile::taken) == 0) {
            tiles[p] = Tile::taken;
          }
        } break;
        case Tile::taken: {
          if (g.count_queen_path(p, Tile::taken) > 4) {
            tiles[p] = Tile::empty;
          }
        } break;
      }
    }
  }
  g.tiles = tiles;
  return g != init_g;
}

auto search(const Grid& init_grid) {
  Grid g1{init_grid};
  while (step_part1(g1)) {
  }
  Grid g2{init_grid};
  while (step_part2(g2)) {
  }
  return std::pair{g1.count(Tile::taken), g2.count(Tile::taken)};
}

Grid parse_grid(std::string_view path) {
  Grid g;
  {
    std::istringstream is{aoc::slurp_file(path)};
    Vec2 pos;
    for (std::string line; std::getline(is, line) and not line.empty(); pos.y() += 1) {
      if (not g.width) {
        g.width = line.size();
      } else if (line.size() != g.width) {
        throw std::runtime_error("every line must be of same width");
      }
      pos.x() = 0;
      for (char ch : line) {
        Tile tile;
        switch (ch) {
          case std::to_underlying(Tile::floor):
          case std::to_underlying(Tile::empty):
          case std::to_underlying(Tile::taken): {
            tile = {ch};
          } break;
          default: {
            throw std::runtime_error(std::format("invalid line {}", line));
          } break;
        }
        g.tiles[pos] = tile;
        pos.x() += 1;
      }
    }
  }
  return g;
}

int main() {
  const auto [part1, part2]{search(parse_grid("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
