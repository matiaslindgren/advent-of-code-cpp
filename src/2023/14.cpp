import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  round = 'O',
  pound = '#',
  ground = '.',
};

struct Grid2D {
  std::vector<Tile> tiles;
  std::size_t width{};
  std::size_t height{};

 private:
  auto bidirectional_range(const auto begin, const auto end) const {
    const auto v{views::iota(begin, end)};
    return views::zip(v, views::reverse(v));
  }
  auto y_range() const {
    return bidirectional_range(1uz, height - 1);
  }
  auto x_range() const {
    return bidirectional_range(1uz, width - 1);
  }

 public:
  void append_padding() {
    tiles.append_range(std::vector<Tile>(width, Tile::pound));
    ++height;
  }

  void tilt_up() {
    tilt(-width);
  }
  void tilt_left() {
    tilt(-1);
  }
  void tilt_down() {
    tilt(width);
  }
  void tilt_right() {
    tilt(1);
  }

  void cycle() {
    tilt_up();
    tilt_left();
    tilt_down();
    tilt_right();
  }

  auto load() const {
    long l{};
    for (const auto [y, y_rev] : y_range()) {
      for (const auto [x, _] : x_range()) {
        if (tiles[y * width + x] == Tile::round) {
          l += y_rev;
        }
      }
    }
    return l;
  }

  constexpr auto operator==(const Grid2D& other) const {
    return ranges::equal(tiles, other.tiles);
  }
  constexpr auto operator<(const Grid2D& other) const {
    return ranges::lexicographical_compare(tiles, other.tiles);
  }

 private:
  std::size_t roll(auto i1, const auto delta) const {
    for (auto i2{i1 + delta}; i2 < tiles.size() && tiles[i2] == Tile::ground; i2 += delta) {
      i1 = i2;
    }
    return i1;
  }

  void tilt(const auto delta) {
    for (const auto [y_fwd, y_rev] : y_range()) {
      for (const auto [x_fwd, x_rev] : x_range()) {
        const auto y{delta == width ? y_rev : y_fwd};
        const auto x{delta == 1 ? x_rev : x_fwd};
        const auto i{y * width + x};
        if (tiles[i] == Tile::round) {
          std::swap(tiles[i], tiles[roll(i, delta)]);
        }
      }
    }
  }
};

std::istream& operator>>(std::istream& is, Grid2D& g) {
  for (std::string line; std::getline(is, line) && !line.empty(); ++g.height) {
    line = "#" + line + "#";
    if (!g.width) {
      g.width = line.size();
      g.append_padding();
    } else if (line.size() != g.width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    for (std::stringstream ls{line}; is && ls;) {
      if (std::underlying_type_t<Tile> ch; ls >> ch) {
        switch (ch) {
          case std::to_underlying(Tile::round):
          case std::to_underlying(Tile::pound):
          case std::to_underlying(Tile::ground): {
            g.tiles.push_back(Tile{ch});
          } break;
          default: {
            is.setstate(std::ios_base::failbit);
          } break;
        }
      }
    }
  }
  if (g.width && g.height) {
    g.append_padding();
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing grid");
}

auto find_part1(Grid2D grid) {
  grid.tilt_up();
  return grid.load();
}

auto find_part2(Grid2D grid) {
  std::set<Grid2D> seen;
  auto grid2{grid};
  while (!seen.contains(grid2)) {
    seen.insert(grid2);
    grid2.cycle();
  }

  std::vector<Grid2D> repeating;
  for (grid = grid2; repeating.empty() || grid2 != grid;) {
    seen.erase(grid);
    repeating.push_back(grid);
    grid.cycle();
  }

  const auto i{(1'000'000'000uz - seen.size()) % repeating.size()};
  return repeating[i].load();
}

int main() {
  aoc::init_io();

  Grid2D grid;
  std::cin >> grid;

  const auto part1{find_part1(grid)};
  const auto part2{find_part2(grid)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
