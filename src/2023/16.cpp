#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Direction : unsigned char {
  N,
  E,
  S,
  W,
};

struct Beam {
  Direction dir;
  Vec2 pos;
};

enum class Tile : char {
  split_NS = '|',
  split_EW = '-',
  mirror_NE = '/',
  mirror_NW = '\\',
  empty = '.',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  int width{};
  int height{};
};

auto count_energized(const Grid& grid, const Beam entry) {
  std::unordered_map<Vec2, Direction> energized;
  for (std::deque<Beam> q{{entry}}; not q.empty(); q.pop_front()) {
    Beam b{q.front()};
    if (not grid.tiles.contains(b.pos)) {
      continue;
    }

    if (energized.contains(b.pos) and b.dir == energized.at(b.pos)) {
      continue;
    }
    energized[b.pos] = b.dir;

    Tile tile{grid.tiles.at(b.pos)};

    const Beam n_beam{Direction::N, b.pos - Vec2(0, 1)};
    const Beam e_beam{Direction::E, b.pos + Vec2(1, 0)};
    const Beam s_beam{Direction::S, b.pos + Vec2(0, 1)};
    const Beam w_beam{Direction::W, b.pos - Vec2(1, 0)};

    switch (b.dir) {
      case Direction::N: {
        switch (tile) {
          case Tile::split_EW: {
            q.push_back(e_beam);
            q.push_back(w_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(e_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(w_beam);
          } break;
          case Tile::split_NS:
          case Tile::empty: {
            q.push_back(n_beam);
          } break;
        }
      } break;
      case Direction::E: {
        switch (tile) {
          case Tile::split_NS: {
            q.push_back(n_beam);
            q.push_back(s_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(n_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(s_beam);
          } break;
          case Tile::split_EW:
          case Tile::empty: {
            q.push_back(e_beam);
          } break;
        }
      } break;
      case Direction::S: {
        switch (tile) {
          case Tile::split_EW: {
            q.push_back(e_beam);
            q.push_back(w_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(w_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(e_beam);
          } break;
          case Tile::split_NS:
          case Tile::empty: {
            q.push_back(s_beam);
          } break;
        }
      } break;
      case Direction::W: {
        switch (tile) {
          case Tile::split_NS: {
            q.push_back(n_beam);
            q.push_back(s_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(s_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(n_beam);
          } break;
          case Tile::split_EW:
          case Tile::empty: {
            q.push_back(w_beam);
          } break;
        }
      } break;
    }
  }
  return energized.size();
}

auto find_part1(const Grid& grid) {
  return count_energized(grid, Beam{Direction::E, Vec2()});
}

auto find_part2(const Grid& grid) {
  return ranges::max(views::transform(
      views::iota(0, std::max(grid.width, grid.height)),
      [&grid](int i) {
        return std::max({
            count_energized(grid, Beam{Direction::S, Vec2(i, 0)}),
            count_energized(grid, Beam{Direction::N, Vec2(i, grid.height)}),
            count_energized(grid, Beam{Direction::E, Vec2(0, i)}),
            count_energized(grid, Beam{Direction::W, Vec2(grid.width, i)}),
        });
      }
  ));
}

auto parse_grid(std::string_view path) {
  Grid g{};
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::split_NS):
        case std::to_underlying(Tile::split_EW):
        case std::to_underlying(Tile::mirror_NE):
        case std::to_underlying(Tile::mirror_NW):
        case std::to_underlying(Tile::empty): {
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
  }
  if (p == Vec2()) {
    throw std::runtime_error("empty input");
  }
  g.height = p.y();
  return g;
}

int main() {
  const Grid g{parse_grid("/dev/stdin")};

  const auto part1{find_part1(g)};
  const auto part2{find_part2(g)};

  std::println("{} {}", part1, part2);

  return 0;
}
