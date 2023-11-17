import std;

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
  if (char dir; is >> dir && (dir == 'L' || dir == 'R')) {
    if (int steps; is >> steps) {
      move = {(dir == 'L' ? Move::Left : Move::Right), steps};
      return is.ignore(1, ',');
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Move");
}

struct Vec2D {
  int y;
  int x;
  bool operator==(const Vec2D&) const = default;
  int distance() const {
    return std::abs(y) + std::abs(x);
  }
  Vec2D rotate_left() const {
    return {-x, y};
  }
  Vec2D rotate_right() const {
    return {x, -y};
  }
  Vec2D operator+(const Vec2D& rhs) {
    return {y + rhs.y, x + rhs.x};
  }
};

using Path = std::vector<Vec2D>;
using Moves = std::vector<Move>;

Path walk(const Moves& moves) {
  std::vector<Vec2D> path = {{}};
  Vec2D facing = {1, 0};
  for (const auto& move : moves) {
    switch (move.direction) {
      case Move::Left: {
        facing = facing.rotate_left();
      } break;
      case Move::Right: {
        facing = facing.rotate_right();
      } break;
    }
    for (int n{}; n < move.steps; ++n) {
      path.push_back(path.back() + facing);
    }
  }
  return path;
}

int find_part1(const Path& path) {
  return path.back().distance();
}

int find_part2(const Path& path) {
  for (auto it{path.begin()}; it != path.end(); ++it) {
    if (ranges::find(it + 1, path.end(), *it) != path.end()) {
      return it->distance();
    }
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto moves = views::istream<Move>(std::cin) | ranges::to<std::vector<Move>>();

  const auto path = walk(moves);

  const auto part1{find_part1(path)};
  const auto part2{find_part2(path)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
