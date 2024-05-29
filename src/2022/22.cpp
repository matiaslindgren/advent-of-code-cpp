#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::is_digit;
using Vec2 = ndvec::vec2<int>;
using Vec3 = ndvec::vec3<int>;

constexpr int face_size{50};

enum struct Tile : char {
  empty = ' ',
  floor = '.',
  stone = '#',
};

struct Move {
  enum : unsigned char {
    left,
    right,
    forward,
  } direction{};
  int steps{};
};

struct Face {
  Vec3 pos;
  Vec3 dx;
  Vec3 dy;
};

int wrap(int x, int n) {
  return ((x % n) + n) % n;
}

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  std::unordered_map<Vec2, Face> faces;
  std::unordered_map<Vec3, std::unordered_map<Vec3, Vec2>> edges;
  int width{};
  int height{};

  [[nodiscard]]
  Tile get(const Vec2& p) const {
    if (tiles.contains(p)) {
      return tiles.at(p);
    }
    return Tile::empty;
  }

  [[nodiscard]]
  bool is_inside(const Vec2& p) const {
    return get(p) != Tile::empty;
  }

  [[nodiscard]]
  Vec2 walk_until_tile(Vec2 pos, Vec2 dir) const {
    if (dir != Vec2()) {
      for (Tile t{Tile::empty}; t == Tile::empty; t = get(pos)) {
        pos.x() = wrap(pos.x() + dir.x(), width);
        pos.y() = wrap(pos.y() + dir.y(), height);
      }
    }
    return pos;
  }

  void step_2d(Vec2& pos, const Vec2& dir) const {
    if (Vec2 new_pos{walk_until_tile(pos, dir)}; get(new_pos) == Tile::floor) {
      pos = new_pos;
    }
  }

  // 2D <=> 3D mapping idea from
  // https://www.reddit.com/r/adventofcode/comments/zsct8w/comment/j18dzaa
  // accessed 2024-05-20
  void step_3d(Vec2& pos, Vec2& dir) const {
    Vec2 new_pos{pos + dir};
    Vec2 new_dir{dir};

    if (not is_inside(new_pos)) {
      constexpr Vec2 fs(face_size, face_size);

      Face face{faces.at((pos / fs) * fs)};

      const int face_x{wrap(pos.x(), fs.x())};
      const int face_y{wrap(pos.y(), fs.y())};

      Vec3 here{
          face.pos + face.dx * Vec3(face_x, face_x, face_x) + face.dy * Vec3(face_y, face_y, face_y)
      };
      Vec3 n{face.dy.cross(face.dx)};
      new_pos = edges.at(here).at(
          face.dx * Vec3(-dir.x(), -dir.x(), -dir.x())
          + face.dy * Vec3(-dir.y(), -dir.y(), -dir.y())
      );

      face = faces.at((new_pos / fs) * fs);
      new_dir = Vec2(face.dx.dot(n), face.dy.dot(n));
    }

    if (get(new_pos) == Tile::floor) {
      pos = new_pos;
      dir = new_dir;
    }
  }
};

enum struct WrapType : unsigned char {
  square,
  cube,
};

auto search(Grid grid, const auto& moves, WrapType wrap_type) {
  Vec2 dir(1, 0);
  Vec2 pos{grid.walk_until_tile(Vec2(), dir)};
  for (Move m : moves) {
    switch (m.direction) {
      case Move::left: {
        dir.rotate_left();
      } break;
      case Move::right: {
        dir.rotate_right();
      } break;
      case Move::forward: {
        for (int s{}; s < m.steps; ++s) {
          switch (wrap_type) {
            case WrapType::square: {
              grid.step_2d(pos, dir);
            } break;
            case WrapType::cube: {
              grid.step_3d(pos, dir);
            } break;
          }
        }
      } break;
    }
  }
  auto pos_value{1000 * (pos.y() + 1) + 4 * (pos.x() + 1)};
  auto dir_value{(dir == Vec2(0, 1)) + 2 * (dir == Vec2(-1, 0)) + 3 * (dir == Vec2(0, -1))};
  return pos_value + dir_value;
}

std::istream& operator>>(std::istream& is, Move& m) {
  if (char ch{}; is >> ch) {
    if (ch == 'R') {
      m = Move{.direction = Move::right};
    } else if (ch == 'L') {
      m = Move{.direction = Move::left};
    } else if (is_digit(ch)) {
      m = Move{.direction = Move::forward};
      do {
        m.steps = 10 * m.steps + (ch - '0');
      } while (is_digit(is.peek()) and is >> ch);
    } else {
      throw std::runtime_error(std::format("invalid move '{}'", ch));
    }
  }
  return is;
}

auto parse_input(std::string_view path) {
  auto lines{aoc::slurp_lines(path)};
  if (lines.size() < 3) {
    throw std::runtime_error("expected at least 3 input lines");
  }

  Grid grid;
  {
    Vec2 p;
    for (const std::string& line : lines | views::take(lines.size() - 2)) {
      p.x() = 0;
      for (char ch : line) {
        Tile t{};
        switch (ch) {
          case std::to_underlying(Tile::empty):
          case std::to_underlying(Tile::floor):
          case std::to_underlying(Tile::stone): {
            t = {ch};
          } break;
          default:
            throw std::runtime_error(std::format("invalid tile '{}'", ch));
        }
        grid.tiles[p] = t;
        p.x() += 1;
      }
      grid.width = std::max(grid.width, p.x());
      p.y() += 1;
    }
    grid.height = p.y();
  }

  // 2D <=> 3D mapping idea from
  // https://www.reddit.com/r/adventofcode/comments/zsct8w/comment/j18dzaa
  // accessed 2024-05-20
  for (std::deque q{std::tuple{
           Vec2(50, 0),
           Vec3(0, 0, 0),
           Vec3(0, 1, 0),
           Vec3(1, 0, 0),
       }};
       not q.empty();
       q.pop_front()) {
    auto [pos, face_pos, dx, dy]{q.front()};
    if (not grid.is_inside(pos) or grid.faces.contains(pos)) {
      continue;
    }
    grid.faces[pos] = Face{.pos = face_pos, .dx = dx, .dy = dy};
    Vec3 fs(face_size - 1, face_size - 1, face_size - 1);
    for (int r{}; r < face_size; ++r) {
      Vec3 r3(r, r, r);
      grid.edges[face_pos + dy * r3][dy.cross(dx)] = pos + Vec2(0, r);
      grid.edges[face_pos + dy * r3 + dx * fs][dy.cross(dx)] = pos + Vec2(fs.x(), r);
      grid.edges[face_pos + dx * r3][dy.cross(dx)] = pos + Vec2(r, 0);
      grid.edges[face_pos + dx * r3 + dy * fs][dy.cross(dx)] = pos + Vec2(r, fs.y());
    }
    q.emplace_back(pos + Vec2(0, face_size), face_pos + dy * fs, dx, dy.cross(dx));
    q.emplace_back(pos - Vec2(0, face_size), face_pos + dy.cross(dx) * fs, dx, dx.cross(dy));
    q.emplace_back(pos + Vec2(face_size, 0), face_pos + dx * fs, dy.cross(dx), dy);
    q.emplace_back(pos - Vec2(face_size, 0), face_pos + dy.cross(dx) * fs, dx.cross(dy), dy);
  }

  std::vector<Move> moves;
  {
    std::istringstream ls{lines.back()};
    moves = views::istream<Move>(ls) | ranges::to<std::vector>();
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing moves line '{}'", lines.back()));
    }
  }

  return std::pair{grid, moves};
}

int main() {
  const auto [grid, moves]{parse_input("/dev/stdin")};

  const auto part1{search(grid, moves, WrapType::square)};
  const auto part2{search(grid, moves, WrapType::cube)};

  std::println("{} {}", part1, part2);

  return 0;
}
