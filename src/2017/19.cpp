import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile {
  bend_north_east,
  bend_east_south,
  bend_south_west,
  bend_west_north,
  pipe_vertical,
  pipe_horizontal,
  letter,
  empty,
};

struct Grid2D {
  std::size_t height{};
  std::size_t width{};
  std::vector<Tile> tiles;
  std::unordered_map<std::size_t, char> letters;

  explicit Grid2D(const auto h, const auto w) : height{h}, width{w}, tiles(h * w, Tile::empty) {
  }

  auto start_state() const {
    const auto i{ranges::find(tiles, Tile::pipe_vertical) - tiles.begin()};
    const auto [y, x]{std::lldiv(i, width)};
    return std::tuple{y, x, 1, 0};
  }

  auto index(const auto y, const auto x) const {
    return y * width + x;
  }

  // TODO
  const Tile& get(const auto y, const auto x) const {
    return tiles.at(index(y, x));
  }
  Tile& get(const auto y, const auto x) {
    return tiles.at(index(y, x));
  }

  const char& get_letter(const auto y, const auto x) const {
    return letters.at(index(y, x));
  }
  char& get_letter(const auto y, const auto x) {
    return letters[index(y, x)];
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
  if (t == '+') {
    const char n{lines[y - 1][x]};
    const char e{lines[y][x + 1]};
    const char s{lines[y + 1][x]};
    const char w{lines[y][x - 1]};
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
  } else if (t == '|') {
    return Tile::pipe_vertical;
  } else if (t == '-') {
    return Tile::pipe_horizontal;
  } else if (t != ' ') {
    return Tile::letter;
  }
  return Tile::empty;
}

Grid2D parse_grid(const auto& lines) {
  Grid2D g(lines.size(), lines.front().size());
  for (auto y{1uz}; y < g.height - 1; ++y) {
    for (auto x{1uz}; x < g.width - 1; ++x) {
      const auto tile{parse_tile(lines, y, x)};
      if (tile == Tile::letter) {
        g.get_letter(y, x) = lines[y][x];
      }
      g.get(y, x) = tile;
    }
  }
  return g;
}

auto traverse_diagram(const Grid2D& grid) {
  std::string seen;
  int steps{};
  for (auto [y, x, dy, dx] = grid.start_state(); grid.get(y, x) != Tile::empty; ++steps) {
    const auto tile{grid.get(y, x)};
    if (tile == Tile::bend_north_east or tile == Tile::bend_south_west) {
      dy = std::exchange(dx, dy);
    } else if (tile == Tile::bend_west_north or tile == Tile::bend_east_south) {
      dy = -std::exchange(dx, -dy);
    } else if (tile == Tile::letter) {
      seen.push_back(grid.get_letter(y, x));
    }
    x += dx;
    y += dy;
  }
  return std::pair{seen, steps};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const Grid2D grid{parse_grid(parse_lines(input))};

  const auto [part1, part2] = traverse_diagram(grid);
  std::print("{} {}\n", part1, part2);

  return 0;
}
