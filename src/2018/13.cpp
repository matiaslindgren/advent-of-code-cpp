#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : unsigned char {
  intersection,
  turn_north_east,
  turn_east_south,
  turn_south_west,
  turn_west_north,
  path_vertical,
  path_horizontal,
  cart,
  empty,
};

enum class Turn : unsigned char {
  left = 0,
  straight = 1,
  right = 2,
};

Turn next(Turn t) {
  return Turn{static_cast<unsigned char>((std::to_underlying(t) + 1) % 3)};
}

using Vec2 = ndvec::vec2<int>;

struct Cart {
  Vec2 pos;
  Vec2 dir;
  Turn turn{};

  [[nodiscard]]
  std::string pos2str() const {
    return std::format("{},{}", pos.x(), pos.y());
  }

  auto operator<=>(const Cart&) const = default;
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
};

Cart turn(const auto& grid, Cart cart) {
  Turn turn{};
  switch (grid.tiles.at(cart.pos)) {
    case Tile::intersection: {
      turn = std::exchange(cart.turn, next(cart.turn));
    } break;
    case Tile::turn_north_east: {
      turn = cart.dir.x() ? Turn::right : Turn::left;
    } break;
    case Tile::turn_east_south: {
      turn = cart.dir.y() ? Turn::right : Turn::left;
    } break;
    case Tile::turn_south_west: {
      turn = cart.dir.x() ? Turn::right : Turn::left;
    } break;
    case Tile::turn_west_north: {
      turn = cart.dir.y() ? Turn::right : Turn::left;
    } break;
    case Tile::path_vertical:
    case Tile::path_horizontal: {
      turn = Turn::straight;
    } break;
    default: {
      throw std::runtime_error("cart in unknown state");
    } break;
  }
  if (turn == Turn::left) {
    cart.dir.rotate_left();
  } else if (turn == Turn::right) {
    cart.dir.rotate_right();
  }
  return cart;
}

auto run(auto grid, auto carts) {
  std::vector<Cart> crashed;

  while (carts.size() > 1) {
    ranges::sort(carts);
    auto pos2cart{
        views::transform(carts, [&](const Cart& c) { return std::pair{c.pos, c}; })
        | ranges::to<std::unordered_map>()
    };
    for (Cart cart : std::exchange(carts, {})) {
      if (pos2cart.erase(cart.pos) == 0) {
        continue;
      }
      cart.pos += cart.dir;
      cart = turn(grid, cart);
      if (const auto other{pos2cart.find(cart.pos)}; other != pos2cart.end()) {
        crashed.push_back(cart);
        crashed.push_back(other->second);
        pos2cart.erase(other);
      } else {
        pos2cart[cart.pos] = cart;
      }
    }
    carts.append_range(pos2cart | views::values);
  }

  if (crashed.empty()) {
    throw std::runtime_error("at least one cart should have crashed");
  }
  if (carts.size() != 1) {
    throw std::runtime_error("one uncrashed cart should be left");
  }
  return std::pair{crashed.front().pos2str(), carts.front().pos2str()};
}

char get_char(const auto& lines, int x, int y) {
  if (0 <= x and x < lines.size() and 0 <= y and y < lines.at(0).size()) {
    return lines.at(y).at(x);
  }
  return ' ';
}

Vec2 parse_direction(char ch) {
  switch (ch) {
    case '>':
      return Vec2(1, 0);
    case 'v':
      return Vec2(0, 1);
    case '<':
      return Vec2(-1, 0);
    case '^':
      return Vec2(0, -1);
  }
  throw std::runtime_error("invalid direction");
}

Tile parse_tile(const auto& lines, int x, int y) {
  char t{get_char(lines, x, y)};
  switch (t) {
    case '+':
      return Tile::intersection;
    case '\\':
    case '/': {
      bool n{get_char(lines, x, y - 1) != ' '};
      bool e{get_char(lines, x + 1, y) != ' '};
      bool s{get_char(lines, x, y + 1) != ' '};
      bool w{get_char(lines, x - 1, y) != ' '};
      if (t == '\\') {
        if (n and e) {
          return Tile::turn_north_east;
        }
        if (s and w) {
          return Tile::turn_south_west;
        }
      }
      if (t == '/') {
        if (w and n) {
          return Tile::turn_west_north;
        }
        if (e and s) {
          return Tile::turn_east_south;
        }
      }
    } break;
    case '|':
      return Tile::path_vertical;
    case '-':
      return Tile::path_horizontal;
    case '>':
    case 'v':
    case '<':
    case '^':
      return Tile::cart;
  }
  return Tile::empty;
}

auto parse_state(std::string_view path) {
  const auto lines{aoc::slurp_lines(path)};
  Grid g;
  std::vector<Cart> carts;
  for (Vec2 p; p.y() < lines.size(); p.y() += 1) {
    for (p.x() = 0; p.x() < lines.at(p.y()).size(); p.x() += 1) {
      Tile t{parse_tile(lines, p.x(), p.y())};
      if (t == Tile::cart) {
        Vec2 dir{parse_direction(get_char(lines, p.x(), p.y()))};
        carts.push_back(
            Cart{
                .pos = p,
                .dir = dir,
                .turn = Turn::left,
            }
        );
        t = dir.y() == 0 ? Tile::path_horizontal : Tile::path_vertical;
      }
      g.tiles[p] = t;
    }
  }
  return std::pair{g, carts};
}

int main() {
  const auto [grid, carts]{parse_state("/dev/stdin")};

  const auto [part1, part2]{run(grid, carts)};
  std::println("{} {}", part1, part2);

  return 0;
}
