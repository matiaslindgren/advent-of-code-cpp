import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile {
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

enum class Turn : int {
  left = 0,
  straight = 1,
  right = 2,
};

using Vec2 = aoc::Vec2<int>;

struct Cart {
  Vec2 pos{}, dir{};
  Turn turn{};

  std::string pos2str() const {
    return std::format("{},{}", pos.x() - 1, pos.y() - 1);
  }
  auto operator<=>(const Cart&) const = default;
};

struct Grid2D {
  std::size_t height{}, width{};
  std::vector<Tile> tiles;

  explicit Grid2D(const auto h, const auto w) : height{h}, width{w}, tiles(h * w, Tile::empty) {
  }
  auto index(const Vec2& v) const {
    return v.y() * width + v.x();
  }
  // TODO deduce this
  const auto& get_tile(const Vec2& v) const {
    return tiles.at(index(v));
  }
  auto& get_tile(const Vec2& v) {
    return tiles.at(index(v));
  }
};

auto parse_lines(std::istream& is) {
  using std::operator""s;
  std::vector<std::string> lines;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    line = " "s + line + " "s;
    if (lines.empty()) {
      lines.emplace_back(line.size(), ' ');
    }
    lines.push_back(line);
  }
  if (not lines.empty() and (is or is.eof())) {
    lines.emplace_back(lines.front().size(), ' ');
    return lines;
  }
  throw std::runtime_error("failed parsing lines");
}

Tile parse_tile(const auto& lines, const auto y, const auto x) {
  const char t{lines[y][x]};
  switch (t) {
    case '+':
      return Tile::intersection;
    case '\\':
    case '/': {
      const bool n{lines[y - 1][x] != ' '};
      const bool e{lines[y][x + 1] != ' '};
      const bool s{lines[y + 1][x] != ' '};
      const bool w{lines[y][x - 1] != ' '};
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

Vec2 parse_direction(const char ch) {
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

std::pair<Grid2D, std::vector<Cart>> parse_state(const auto& lines) {
  Grid2D g(lines.size(), lines.front().size());
  std::vector<Cart> carts;
  for (Vec2 p(0, 1); p.y() < g.height - 1; ++p.y()) {
    for (p.x() = 1; p.x() < g.width - 1; ++p.x()) {
      auto tile{parse_tile(lines, p.y(), p.x())};
      if (tile == Tile::cart) {
        const auto dir{parse_direction(lines[p.y()][p.x()])};
        carts.push_back(Cart{
            .pos = p,
            .dir = dir,
            .turn = Turn::left,
        });
        tile = dir.y() ? Tile::path_vertical : Tile::path_horizontal;
      }
      g.get_tile(p) = tile;
    }
  }
  return {g, carts};
}

Cart turn(const auto& grid, Cart cart) {
  Turn turn;
  switch (grid.get_tile(cart.pos)) {
    case Tile::intersection: {
      turn = std::exchange(cart.turn, Turn{(std::to_underlying(cart.turn) + 1) % 3});
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

  const auto pos_index{[&grid](const Cart& c) { return grid.index(c.pos); }};

  while (carts.size() > 1) {
    ranges::sort(carts);
    auto pos2cart{
        views::transform(
            carts,
            [&](const auto& c) {
              return std::pair{pos_index(c), c};
            }
        )
        | ranges::to<std::unordered_map<int, Cart>>()
    };
    for (Cart cart : std::exchange(carts, {})) {
      if (pos2cart.erase(pos_index(cart)) == 0) {
        continue;
      }
      cart.pos += cart.dir;
      cart = turn(grid, cart);
      if (const auto other{pos2cart.find(pos_index(cart))}; other != pos2cart.end()) {
        crashed.push_back(cart);
        crashed.push_back(other->second);
        pos2cart.erase(other);
      } else {
        pos2cart[pos_index(cart)] = cart;
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

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto [grid, carts]{parse_state(parse_lines(input))};

  const auto [part1, part2]{run(grid, carts)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
