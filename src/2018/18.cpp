#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  open = '.',
  tree = '|',
  yard = '#',
};

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::open):
      case std::to_underlying(Tile::tree):
      case std::to_underlying(Tile::yard): {
        tile = {ch};
      } break;
      default: {
        is.setstate(std::ios_base::failbit);
      } break;
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Tile");
}

constexpr auto square_points(const auto begin, const auto end) {
  auto&& side{views::iota(begin, end)};
  return my_std::views::cartesian_product(side, side);
}

struct Grid2D {
  std::vector<Tile> tiles;
  std::size_t width{};
  std::size_t height{};

  void append_padding() {
    tiles.append_range(views::repeat(Tile::open, width));
    ++height;
  }

  auto&& get(this auto&& self, auto y, auto x) {
    return self.tiles.at(y * self.width + x);
  }

  Grid2D step() const {
    Grid2D g{*this};
    for (auto [y, x] : square_points(1uz, width - 1)) {
      int trees{}, yards{};
      for (auto [dy, dx] : square_points(-1, 2)) {
        if (dy == 0 and dx == 0) {
          continue;
        }
        const auto tile{get(y + dy, x + dx)};
        trees += tile == Tile::tree;
        yards += tile == Tile::yard;
      }
      switch (get(y, x)) {
        case Tile::open: {
          if (trees >= 3) {
            g.get(y, x) = Tile::tree;
          }
        } break;
        case Tile::tree: {
          if (yards >= 3) {
            g.get(y, x) = Tile::yard;
          }
        } break;
        case Tile::yard: {
          if (yards > 0 and trees > 0) {
            g.get(y, x) = Tile::yard;
          } else {
            g.get(y, x) = Tile::open;
          }
        } break;
      }
    }
    return g;
  }
};

Grid2D parse_grid(std::istream& is) {
  Grid2D g;
  for (std::string line; std::getline(is, line) and not line.empty(); ++g.height) {
    line.insert(line.begin(), std::to_underlying(Tile::open));
    line.insert(line.end(), std::to_underlying(Tile::open));
    if (not g.width) {
      g.width = line.size();
      g.append_padding();
    } else if (line.size() != g.width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    std::istringstream ls{line};
    g.tiles.append_range(views::istream<Tile>(ls));
  }
  if (is or is.eof()) {
    g.append_padding();
    return g;
  }
  throw std::runtime_error("failed parsing Grid");
}

auto simulate(Grid2D grid, const auto t_limit) {
  std::vector<decltype(grid.tiles)> seen;
  for (int t{}; t < t_limit; ++t) {
    if (const auto prev{ranges::find(seen, grid.tiles)}; prev != seen.end()) {
      const auto cycle_len{ranges::distance(prev, seen.end())};
      grid.tiles = *ranges::next(prev, (t_limit - t) % cycle_len);
      break;
    }
    seen.push_back(grid.tiles);
    grid = grid.step();
  }
  return ranges::count(grid.tiles, Tile::tree) * ranges::count(grid.tiles, Tile::yard);
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto grid{parse_grid(input)};

  const auto part1{simulate(grid, 10)};
  const auto part2{simulate(grid, 1'000'000'000)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
