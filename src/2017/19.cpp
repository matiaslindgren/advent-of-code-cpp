#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : unsigned char {
  bend_north_east,
  bend_east_south,
  bend_south_west,
  bend_west_north,
  pipe_vertical,
  pipe_horizontal,
  letter,
  empty,
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  std::unordered_map<Vec2, char> letters;

  auto& get_tile(const Vec2& p) {
    return tiles[p];
  }
  [[nodiscard]]
  const auto& get_tile(const Vec2& p) const {
    return tiles.at(p);
  }

  auto& get_letter(const Vec2& p) {
    return letters[p];
  }
  [[nodiscard]]
  const auto& get_letter(const Vec2& p) const {
    return letters.at(p);
  }

  [[nodiscard]]
  Vec2 find_entrance() const {
    auto top_row{
        tiles | views::keys | views::filter([](const Vec2& p) { return p.y() == 0; })
        | ranges::to<std::vector>()
    };
    ranges::sort(top_row);
    for (const Vec2& p : top_row) {
      if (get_tile(p) == Tile::pipe_vertical) {
        return p;
      }
    }
    throw std::runtime_error("top row does not contain a vertical pipe");
  }
};

auto traverse_diagram(Grid grid) {
  std::string seen;
  int steps{};
  for (Vec2 p{grid.find_entrance()}, d(0, 1); grid.get_tile(p) != Tile::empty; ++steps) {
    Tile t{grid.get_tile(p)};
    if (t == Tile::bend_north_east or t == Tile::bend_south_west) {
      d.y() = std::exchange(d.x(), d.y());
    } else if (t == Tile::bend_west_north or t == Tile::bend_east_south) {
      d.y() = -std::exchange(d.x(), -d.y());
    } else if (t == Tile::letter) {
      seen.push_back(grid.get_letter(p));
    }
    p += d;
  }
  return std::pair{seen, steps};
}

std::optional<Tile> parse_bend(char n, char e, char s, char w) {
  if (n != ' ' and e != ' ' and n != '-' and e != '|') {
    return Tile::bend_north_east;
  }
  if (e != ' ' and s != ' ' and e != '|' and s != '-') {
    return Tile::bend_east_south;
  }
  if (s != ' ' and w != ' ' and s != '-' and w != '|') {
    return Tile::bend_south_west;
  }
  if (w != ' ' and n != ' ' and w != '|' and n != '-') {
    return Tile::bend_west_north;
  }
  return std::nullopt;
}

auto parse_grid(std::string_view path) {
  const auto lines{aoc::slurp_lines(path)};

  const auto get_char{[&lines](int x, int y) { return lines.at(y).at(x); }};

  const auto parse_tile{[&get_char](int x, int y) {
    char t{get_char(x, y)};
    if (t == '+') {
      char n{get_char(x, y - 1)};
      char e{get_char(x + 1, y)};
      char s{get_char(x, y + 1)};
      char w{get_char(x - 1, y)};
      if (auto bend{parse_bend(n, e, s, w)}) {
        return bend.value();
      }
    } else if (t == '|') {
      return Tile::pipe_vertical;
    } else if (t == '-') {
      return Tile::pipe_horizontal;
    } else if (t != ' ') {
      return Tile::letter;
    }
    return Tile::empty;
  }};

  Grid g{};
  for (Vec2 p; p.y() < lines.size(); p.y() += 1) {
    for (p.x() = 0; p.x() < lines.at(p.y()).size(); p.x() += 1) {
      Tile t{parse_tile(p.x(), p.y())};
      if (t == Tile::letter) {
        g.get_letter(p) = get_char(p.x(), p.y());
      }
      g.get_tile(p) = t;
    }
  }
  return g;
}

int main() {
  const Grid g{parse_grid("/dev/stdin")};
  const auto [part1, part2]{traverse_diagram(g)};
  std::println("{} {}", part1, part2);
  return 0;
}
