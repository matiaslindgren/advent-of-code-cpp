#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  obstacle = '#',
  ground = '.',
};

using Grid = std::unordered_map<Vec2, Tile>;
using Visited = std::unordered_map<Vec2, std::unordered_set<Vec2>>;

auto find_part2(const Grid& grid, Vec2 init_pos, const Visited& visited) {
  std::unordered_set<Vec2> new_obstacles;
  for (const auto& [start_pos, facings] : visited) {
    Vec2 obstacle{start_pos};
    if (obstacle != init_pos and grid.contains(obstacle)) {
      Grid grid2{grid};
      grid2[obstacle] = Tile::obstacle;
      Visited seen;
      for (Vec2 guard{init_pos}, facing(0, -1);;) {
        if (seen.contains(guard) and seen.at(guard).contains(facing)) {
          new_obstacles.insert(obstacle);
          break;
        }
        seen[guard].insert(facing);
        Vec2 next_pos{guard + facing};
        if (not grid2.contains(next_pos)) {
          break;
        }
        if (grid2.at(next_pos) == Tile::obstacle) {
          facing.rotate_right();
        } else {
          guard = next_pos;
        }
      }
    }
  }
  return new_obstacles.size();
}

auto search(const Grid& grid, const Vec2& start_pos) {
  Visited visited;
  for (Vec2 guard{start_pos}, facing(0, -1);;) {
    visited[guard].insert(facing);
    Vec2 next_pos{guard + facing};

    if (not grid.contains(next_pos)) {
      break;
    }

    if (grid.at(next_pos) != Tile::obstacle) {
      guard = next_pos;
      continue;
    }

    facing.rotate_right();
  }

  return std::pair{visited.size(), find_part2(grid, start_pos, visited)};
}

auto parse_grid(std::string_view path) {
  Grid g{};
  size_t width{};
  Vec2 p;
  std::optional<Vec2> guard_pos;

  for (const std::string& line : aoc::slurp_lines(path)) {
    for (p.x() = 0; char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::obstacle):
        case std::to_underlying(Tile::ground):
          g[p] = {ch};
          break;
        case '^':
          g[p] = Tile::ground;
          guard_pos = p;
          break;
        default:
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
    if (width == 0) {
      width = p.x();
    } else if (width != p.x()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.y() += 1;
  }

  if (not guard_pos) {
    throw std::runtime_error("grid does not contain location of guard ^");
  }

  return std::pair{g, guard_pos.value()};
}

int main() {
  const auto [grid, guard]{parse_grid("/dev/stdin")};
  const auto [part1, part2]{search(grid, guard)};
  std::println("{} {}", part1, part2);
  return 0;
}
