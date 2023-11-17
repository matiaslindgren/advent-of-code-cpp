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
  Vec2D& operator+=(const Vec2D& rhs) {
    y += rhs.y;
    x += rhs.x;
    return *this;
  }
  Vec2D operator*(int factor) const {
    return {factor * y, factor * x};
  }
};

using Moves = std::vector<Move>;

std::pair<Vec2D, Vec2D> walk(const Moves& moves) {
  Vec2D delta = {1, 0};
  Vec2D p1 = {};
  Vec2D p2 = {};
  bool found_part2{false};

  std::vector<Vec2D> visited;
  visited.push_back(p1);

  for (const auto& move : moves) {
    switch (move.direction) {
      case Move::Left: {
        delta = delta.rotate_left();
      } break;
      case Move::Right: {
        delta = delta.rotate_right();
      } break;
    }
    for (int n{}; n < move.steps; ++n) {
      p1 += delta;
      if (!found_part2 && ranges::find(visited, p1) != visited.end()) {
        found_part2 = true;
        p2 = p1;
      } else {
        visited.push_back(p1);
      }
    }
  }

  return {p1, p2};
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto moves = views::istream<Move>(std::cin) | ranges::to<std::vector<Move>>();
  const auto [p1, p2] = walk(moves);
  const auto part1{p1.distance()};
  const auto part2{p2.distance()};
  std::print("{} {}\n", part1, part2);

  return 0;
}
