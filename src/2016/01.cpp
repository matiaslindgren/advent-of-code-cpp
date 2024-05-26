#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

struct Move {
  enum : unsigned char {
    Left,
    Right,
  } direction{};
  int steps{};
};

using Vec2 = ndvec::vec2<int>;
using Path = std::vector<Vec2>;
using Moves = std::vector<Move>;

Path walk(const Moves& moves) {
  std::vector path{Vec2{}};
  Vec2 facing(1, 0);
  for (const Move& move : moves) {
    if (move.direction == Move::Left) {
      facing.rotate_left();
    } else {
      facing.rotate_right();
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
    if (std::ranges::find(it + 1, path.end(), *it) != path.end()) {
      return it->distance(Vec2{});
    }
  }
  throw std::runtime_error("oh no");
}

std::istream& operator>>(std::istream& is, Move& move) {
  bool ok{};
  if (char dir{}; is >> dir) {
    if (dir != ',' or is >> dir) {
      if (dir == 'L' or dir == 'R') {
        if (int steps{}; is >> steps) {
          move = {(dir == 'L' ? Move::Left : Move::Right), steps};
          ok = true;
        }
      }
    }
  }
  if (not ok and not is.eof()) {
    throw std::runtime_error("failed parsing Move");
  }
  return is;
}

int main() {
  const auto moves{aoc::parse_items<Move>("/dev/stdin")};
  const auto path{walk(moves)};

  const auto part1{find_part1(path)};
  const auto part2{find_part2(path)};

  std::println("{} {}", part1, part2);

  return 0;
}
