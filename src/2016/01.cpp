#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Move {
  enum {
    Left,
    Right,
  } direction;
  int steps;
};

std::istream& operator>>(std::istream& is, Move& move) {
  if (char dir; is >> dir and (dir == 'L' or dir == 'R')) {
    if (int steps; is >> steps and is.ignore(1, ',')) {
      move = {(dir == 'L' ? Move::Left : Move::Right), steps};
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Move");
}

using Vec2 = ndvec::vec2<int>;
using Path = std::vector<Vec2>;
using Moves = std::vector<Move>;

Path walk(const Moves& moves) {
  std::vector path{Vec2{}};
  Vec2 facing(1, 0);
  for (const auto& move : moves) {
    switch (move.direction) {
      case Move::Left: {
        facing.rotate_left();
      } break;
      case Move::Right: {
        facing.rotate_right();
      } break;
    }
    for (int n{}; n < move.steps; ++n) {
      path.push_back(path.back() + facing);
    }
  }
  return path;
}

int find_part1(const Path& path) {
  return path.back().distance(Vec2{});
}

int find_part2(const Path& path) {
  for (auto it{path.begin()}; it != path.end(); ++it) {
    if (ranges::find(it + 1, path.end(), *it) != path.end()) {
      return it->distance(Vec2{});
    }
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto moves{views::istream<Move>(input) | ranges::to<std::vector>()};
  const auto path{walk(moves)};

  const auto part1{find_part1(path)};
  const auto part2{find_part2(path)};

  std::println("{} {}", part1, part2);

  return 0;
}
