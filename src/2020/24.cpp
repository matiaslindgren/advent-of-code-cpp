#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using aoc::skip;
using std::operator""s;

enum class Direction : unsigned char {
  east,
  south_east,
  south_west,
  west,
  north_west,
  north_east,
};

Vec2 dir2vec(const Direction dir) {
  switch (dir) {
    case Direction::east:
      return Vec2(1, 0);
    case Direction::south_east:
      return Vec2(0, 1);
    case Direction::south_west:
      return Vec2(-1, 1);
    case Direction::west:
      return Vec2(-1, 0);
    case Direction::north_west:
      return Vec2(0, -1);
    case Direction::north_east:
      return Vec2(1, -1);
  }
}

const auto direction_deltas{
    views::transform(
        std::array{
            Direction::east,
            Direction::south_east,
            Direction::south_west,
            Direction::west,
            Direction::north_west,
            Direction::north_east,
        },
        dir2vec
    )
    | ranges::to<std::vector>()
};

auto step(auto grid) {
  // https://github.com/sophiebits/adventofcode/blob/main/2020/day24.py
  // accessed 2024-04-28
  std::unordered_map<Vec2, int> adjacent;
  for (auto [p, n_flips] : grid) {
    if (n_flips % 2) {
      for (const Vec2& d : direction_deltas) {
        adjacent[p + d] += 1;
      }
    }
  }
  std::unordered_map<Vec2, int> res;
  for (auto [p, n_flips] : grid) {
    if (n_flips % 2) {
      if (auto n_adj{adjacent[p]}; n_adj == 1 or n_adj == 2) {
        res[p] = 1;
      }
    }
  }
  for (auto [p, n_adj] : adjacent) {
    if (n_adj == 2 and grid[p] % 2 == 0) {
      res[p] = 1;
    }
  }
  return res;
}

auto count_black(const auto& grid) {
  return ranges::count_if(grid | views::values, [](int n) { return n % 2 == 1; });
}

auto search(const auto& walks) {
  std::unordered_map<Vec2, int> grid;
  for (const auto& walk : walks) {
    Vec2 p{ranges::fold_left(views::transform(walk, dir2vec), Vec2(), std::plus{})};
    grid[p] += 1;
  }
  auto part1{count_black(grid)};
  for (int i{}; i < 100; ++i) {
    grid = step(grid);
  }
  auto part2{count_black(grid)};
  return std::pair{part1, part2};
}

std::istream& operator>>(std::istream& is, Direction& dir) {
  if (char c1{}; is >> c1) {
    if (c1 == 'e') {
      dir = Direction::east;
    } else if (c1 == 'w') {
      dir = Direction::west;
    } else if (char c2{}; c1 == 'n' and is >> c2 and (c2 == 'w' or c2 == 'e')) {
      dir = (c2 == 'w') ? Direction::north_west : Direction::north_east;
    } else if (char c2{}; c1 == 's' and is >> c2 and (c2 == 'w' or c2 == 'e')) {
      dir = (c2 == 'w') ? Direction::south_west : Direction::south_east;
    } else {
      throw std::runtime_error(std::format("unknown direction '{}'", c1));
    }
  }
  return is;
}

auto parse_walks(std::string_view path) {
  return aoc::slurp_lines(path) | views::transform([](auto&& line) {
           std::istringstream ls{line};
           return views::istream<Direction>(ls) | ranges::to<std::vector>();
         })
         | ranges::to<std::vector>();
}

int main() {
  const auto walks{parse_walks("/dev/stdin")};
  const auto [part1, part2]{search(walks)};
  std::println("{} {}", part1, part2);
  return 0;
}
