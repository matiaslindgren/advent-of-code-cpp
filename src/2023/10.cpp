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

enum class Facing {
  N,
  E,
  S,
  W,
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

  std::pair<Tile, Facing> infer_tile(const Tile w, const Tile n, const Tile e, const Tile s) const {
    const auto w_pipe{w == Tile::EW || w == Tile::NE || w == Tile::SE};
    const auto n_pipe{n == Tile::NS || n == Tile::SE || n == Tile::SW};
    const auto e_pipe{e == Tile::EW || e == Tile::NW || e == Tile::SW};
    const auto s_pipe{s == Tile::NS || s == Tile::NE || s == Tile::NW};

    if (n_pipe && s_pipe) {
      return {Tile::NS, Facing::N};
    }
    if (e_pipe && w_pipe) {
      return {Tile::EW, Facing::E};
    }
    if (n_pipe && e_pipe) {
      return {Tile::NE, Facing::N};
    }
    if (n_pipe && w_pipe) {
      return {Tile::NW, Facing::N};
    }
    if (s_pipe && w_pipe) {
      return {Tile::SW, Facing::S};
    }
    if (s_pipe && e_pipe) {
      return {Tile::SE, Facing::S};
    }

    throw std::runtime_error("tile not part of pipe");
  }

  std::pair<std::size_t, Facing> replace_start() {
    const auto mid{ranges::find(tiles, Tile::start) - tiles.begin()};
    const auto tW{tiles.at(mid - 1)};
    const auto tN{tiles.at(mid - size)};
    const auto tE{tiles.at(mid + 1)};
    const auto tS{tiles.at(mid + size)};
    const auto [tmid, facing] = infer_tile(tW, tN, tE, tS);
    tiles.at(mid) = tmid;
    return {mid, facing};
  }

  auto find_path(const auto start_index, const auto start_face) const {
    std::unordered_set<std::size_t> path{start_index};
    for (auto [i, f] = std::make_pair(start_index, start_face);
         path.size() < 2 || i != start_index;) {
      switch (f) {
        case Facing::N: {
          i -= size;
        } break;
        case Facing::E: {
          i += 1;
        } break;
        case Facing::S: {
          i += size;
        } break;
        case Facing::W: {
          i -= 1;
        } break;
      }
      if (i == start_index) {
        break;
      }
      path.insert(i);
      const auto t{tiles.at(i)};
      switch (f) {
        case Facing::N: {
          switch (t) {
            case Tile::NS: {
              f = Facing::N;
            } break;
            case Tile::SW: {
              f = Facing::W;
            } break;
            case Tile::SE: {
              f = Facing::E;
            } break;
            default:
              throw std::runtime_error("cannot find N facing");
          }
        } break;
        case Facing::E: {
          switch (t) {
            case Tile::EW: {
              f = Facing::E;
            } break;
            case Tile::NW: {
              f = Facing::N;
            } break;
            case Tile::SW: {
              f = Facing::S;
            } break;
            default:
              throw std::runtime_error("cannot find E facing");
          }
        } break;
        case Facing::S: {
          switch (t) {
            case Tile::NS: {
              f = Facing::S;
            } break;
            case Tile::NE: {
              f = Facing::E;
            } break;
            case Tile::NW: {
              f = Facing::W;
            } break;
            default:
              throw std::runtime_error("cannot find S facing");
          }
        } break;
        case Facing::W: {
          switch (t) {
            case Tile::EW: {
              f = Facing::W;
            } break;
            case Tile::NE: {
              f = Facing::N;
            } break;
            case Tile::SE: {
              f = Facing::S;
            } break;
            default:
              throw std::runtime_error("cannot find W facing");
          }
        } break;
      }
    }
    return path;
  }
};

auto count_inner(Grid2D grid, const auto& path) {
  for (auto i{0uz}; i < grid.tiles.size(); ++i) {
    if (!path.contains(i)) {
      grid.tiles.at(i) = Tile::ground;
    }
  }
  return ranges::count_if(
      views::iota(0uz, grid.tiles.size()),
      [prev_angle = Tile::ground, is_in = false, &grid](auto&& i) mutable {
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
  std::ios_base::sync_with_stdio(false);

  Grid2D grid{views::istream<Tile>(std::cin) | ranges::to<std::vector<Tile>>()};
  const auto [start_index, start_face] = grid.replace_start();
  const auto path{grid.find_path(start_index, start_face)};

  const auto part1{path.size() / 2};
  const auto part2{count_inner(grid, path)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
