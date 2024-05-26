// TODO slow
#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  round = 'O',
  pound = '#',
  ground = '.',
};

template <std::integral Int>
auto bidirectional_iota(Int begin, Int end) {
  auto v{views::iota(begin, end)};
  return views::zip(v, views::reverse(v));
}

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  int width{};
  int height{};

  [[nodiscard]]
  Tile get(const Vec2& p) const {
    return tiles.contains(p) ? tiles.at(p) : Tile::pound;
  }

 private:
  [[nodiscard]]
  auto x_range() const {
    return bidirectional_iota(0, width);
  }
  [[nodiscard]]
  auto y_range() const {
    return bidirectional_iota(0, height);
  }

 public:
  void tilt_up() {
    tilt(Vec2(0, -1));
  }
  void tilt_left() {
    tilt(Vec2(-1, 0));
  }
  void tilt_down() {
    tilt(Vec2(0, 1));
  }
  void tilt_right() {
    tilt(Vec2(1, 0));
  }

  void cycle() {
    tilt_up();
    tilt_left();
    tilt_down();
    tilt_right();
  }

  [[nodiscard]]
  auto load() const {
    long l{};
    for (const auto [y, y_back] : y_range()) {
      for (const auto [x, _] : x_range()) {
        if (get(Vec2(x, y)) == Tile::round) {
          l += y_back + 1;
        }
      }
    }
    return l;
  }

  bool operator==(const Grid& rhs) const {
    return tiles == rhs.tiles;
  }

 private:
  [[nodiscard]]
  auto roll(Vec2 p, const Vec2& delta) const {
    while (get(p + delta) == Tile::ground) {
      p += delta;
    }
    return p;
  }

  void tilt(const Vec2& delta) {
    for (const auto [y_forward, y_back] : y_range()) {
      for (const auto [x_forward, x_back] : x_range()) {
        const auto x{delta == Vec2(1, 0) ? x_back : x_forward};
        const auto y{delta == Vec2(0, 1) ? y_back : y_forward};
        if (Vec2 p(x, y); get(p) == Tile::round) {
          std::swap(tiles.at(p), tiles.at(roll(p, delta)));
        }
      }
    }
  }
};

auto find_part1(Grid g) {
  g.tilt_up();
  return g.load();
}

auto find_part2(Grid g1) {
  Grid g2{g1};

  std::vector<Grid> seen;
  while (not ranges::contains(seen, g2)) {
    seen.push_back(g2);
    g2.cycle();
  }

  auto tail_size{seen.size()};
  std::vector<Grid> repeating;
  for (g1 = g2; repeating.empty() or g2 != g1;) {
    tail_size -= 1;
    repeating.push_back(g1);
    g1.cycle();
  }

  g1 = repeating.at((1'000'000'000UZ - tail_size) % repeating.size());
  return g1.load();
}

auto parse_grid(std::string_view path) {
  Grid g{};
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::round):
        case std::to_underlying(Tile::pound):
        case std::to_underlying(Tile::ground): {
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
