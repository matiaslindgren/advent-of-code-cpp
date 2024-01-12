import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  NS = '|',
  EW = '-',
  NE = 'L',
  NW = 'J',
  SW = '7',
  SE = 'F',
  ground = '.',
  start = 'S',
};

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::NS):
      case std::to_underlying(Tile::EW):
      case std::to_underlying(Tile::NE):
      case std::to_underlying(Tile::NW):
      case std::to_underlying(Tile::SW):
      case std::to_underlying(Tile::SE):
      case std::to_underlying(Tile::ground):
      case std::to_underlying(Tile::start):
        tile = {ch};
        return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Tile");
}

struct Grid2D {
  std::vector<Tile> tiles;
  std::size_t size;

  explicit Grid2D(const std::vector<Tile>& input_tiles) {
    const std::size_t input_size = std::sqrt(input_tiles.size());
    size = input_size + 2;
    tiles.resize(size * size, Tile::ground);
    for (auto row{0uz}; row < input_size; ++row) {
      for (auto col{0uz}; col < input_size; ++col) {
        tiles[(row + 1) * size + (col + 1)] = input_tiles[row * input_size + col];
      }
    }
  }

  constexpr std::size_t index_of(const Tile tile) const {
    return ranges::find(tiles, tile) - tiles.begin();
  }

  bool is_connected(const auto src, const auto dst) const {
    if (dst < src) {
      return is_connected(dst, src);
    }
    const auto t1{tiles.at(src)};
    const auto t2{tiles.at(dst)};
    if (dst - src == 1) {
      return (t1 == Tile::EW || t1 == Tile::NE || t1 == Tile::SE || t1 == Tile::start)
             && (t2 == Tile::EW || t2 == Tile::NW || t2 == Tile::SW || t2 == Tile::start);
    } else if (dst - src == size) {
      return (t1 == Tile::NS || t1 == Tile::SW || t1 == Tile::SE || t1 == Tile::start)
             && (t2 == Tile::NS || t2 == Tile::NE || t2 == Tile::NW || t2 == Tile::start);
    }
    return false;
  }

  std::array<std::size_t, 4> adjacent(const std::size_t i) const {
    return {i - 1, i - size, i + 1, i + size};
  }

  auto find_path(const auto start_index) const {
    std::unordered_set<std::size_t> path{start_index};
    for (auto src{start_index};;) {
      auto prev{src};
      for (const auto dst : adjacent(src)) {
        if (!path.contains(dst) && is_connected(src, dst)) {
          src = dst;
          break;
        }
      }
      if (prev == src) {
        break;
      }
      path.insert(src);
    }
    return path;
  }

  void infer_tile(const auto i) {
    for (const auto tile : {Tile::NS, Tile::EW, Tile::NE, Tile::NW, Tile::SW, Tile::SE}) {
      const auto prev{std::exchange(tiles.at(i), tile)};
      const auto adjacent_count{ranges::count_if(adjacent(i), [=, this](auto adj) {
        return this->is_connected(i, adj);
      })};
      if (adjacent_count == 2) {
        return;
      }
      tiles.at(i) = prev;
    }
  }
};

auto count_inner(Grid2D grid, const auto& path) {
  grid.infer_tile(grid.index_of(Tile::start));
  for (auto i{0uz}; i < grid.tiles.size(); ++i) {
    if (!path.contains(i)) {
      grid.tiles.at(i) = Tile::ground;
    }
  }
  return ranges::count_if(
      views::iota(0uz, grid.tiles.size()),
      [prev_angle = Tile::ground, is_in = false, &grid](const auto& i) mutable {
        if (i % grid.size == 0) {
          prev_angle = Tile::ground;
          is_in = false;
        }
        const auto t{grid.tiles.at(i)};
        if (t == Tile::ground) {
          return is_in;
        }
        const bool should_flip{
            t == Tile::NS || (t == Tile::NW && prev_angle == Tile::SE)
            || (t == Tile::SW && prev_angle == Tile::NE)
        };
        if (should_flip) {
          is_in = !is_in;
        } else if (t == Tile::NE || t == Tile::SE) {
          prev_angle = t;
        }
        return false;
      }
  );
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const Grid2D grid{views::istream<Tile>(input) | ranges::to<std::vector>()};
  const auto path{grid.find_path(grid.index_of(Tile::start))};

  const auto part1{path.size() / 2};
  const auto part2{count_inner(grid, path)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
