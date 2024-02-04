import std;
import aoc;

namespace views = std::views;

enum class Tile : char {
  split_NS = '|',
  split_EW = '-',
  mirror_NE = '/',
  mirror_NW = '\\',
  empty = '.',
};

struct Grid2D {
  std::vector<Tile> tiles;
  std::size_t width{};
  std::size_t height{};

  bool is_inside(const auto y, const auto x) const {
    return y < height && x < width;
  }
  auto index(const auto y, const auto x) const {
    return y * width + x;
  }
  const auto& get(const auto y, const auto x) const {
    return tiles[index(y, x)];
  }
};

std::istream& operator>>(std::istream& is, Grid2D& g) {
  for (std::string line; std::getline(is, line) && !line.empty(); ++g.height) {
    if (!g.width) {
      g.width = line.size();
    } else if (line.size() != g.width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    for (std::stringstream ls{line}; is && ls;) {
      if (std::underlying_type_t<Tile> ch; ls >> ch) {
        switch (ch) {
          case std::to_underlying(Tile::split_NS):
          case std::to_underlying(Tile::split_EW):
          case std::to_underlying(Tile::mirror_NE):
          case std::to_underlying(Tile::mirror_NW):
          case std::to_underlying(Tile::empty): {
            g.tiles.push_back(Tile{ch});
          } break;
          default: {
            is.setstate(std::ios_base::failbit);
          } break;
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing grid");
}

struct Beam {
  enum class Direction { N, E, S, W } dir;
  std::size_t y;
  std::size_t x;
};

auto count_energized(const Grid2D& grid, const Beam entry) {
  std::unordered_map<std::size_t, Beam::Direction> energized;
  for (std::deque<Beam> q{{entry}}; !q.empty(); q.pop_front()) {
    const auto [dir, y, x] = q.front();
    if (!grid.is_inside(y, x)) {
      continue;
    }
    {
      const auto pos{grid.index(y, x)};
      if (const auto it{energized.find(pos)}; it != energized.end()) {
        if (const auto old_beam_dir{it->second}; dir == old_beam_dir) {
          continue;
        }
      }
      energized[pos] = dir;
    }

    const auto tile{grid.get(y, x)};

    const Beam n_beam{Beam::Direction::N, y - 1, x + 0};
    const Beam e_beam{Beam::Direction::E, y + 0, x + 1};
    const Beam s_beam{Beam::Direction::S, y + 1, x + 0};
    const Beam w_beam{Beam::Direction::W, y + 0, x - 1};

    // TODO vec2 arithmetic
    switch (dir) {
      case Beam::Direction::N: {
        switch (tile) {
          case Tile::split_EW: {
            q.push_back(e_beam);
            q.push_back(w_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(e_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(w_beam);
          } break;
          case Tile::split_NS:
          case Tile::empty: {
            q.push_back(n_beam);
          } break;
        }
      } break;
      case Beam::Direction::E: {
        switch (tile) {
          case Tile::split_NS: {
            q.push_back(n_beam);
            q.push_back(s_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(n_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(s_beam);
          } break;
          case Tile::split_EW:
          case Tile::empty: {
            q.push_back(e_beam);
          } break;
        }
      } break;
      case Beam::Direction::S: {
        switch (tile) {
          case Tile::split_EW: {
            q.push_back(e_beam);
            q.push_back(w_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(w_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(e_beam);
          } break;
          case Tile::split_NS:
          case Tile::empty: {
            q.push_back(s_beam);
          } break;
        }
      } break;
      case Beam::Direction::W: {
        switch (tile) {
          case Tile::split_NS: {
            q.push_back(n_beam);
            q.push_back(s_beam);
          } break;
          case Tile::mirror_NE: {
            q.push_back(s_beam);
          } break;
          case Tile::mirror_NW: {
            q.push_back(n_beam);
          } break;
          case Tile::split_EW:
          case Tile::empty: {
            q.push_back(w_beam);
          } break;
        }
      } break;
    }
  }
  return energized.size();
}

auto find_part1(const Grid2D& grid) {
  return count_energized(grid, {Beam::Direction::E, 0, 0});
}

auto find_part2(const Grid2D& grid) {
  const auto w{grid.width};
  const auto h{grid.height};
  auto max{0uz};
  for (const auto y : views::iota(1uz, h - 1)) {
    max = std::max(max, count_energized(grid, {Beam::Direction::E, y, 0}));
    max = std::max(max, count_energized(grid, {Beam::Direction::W, y, w - 1}));
  }
  for (const auto x : views::iota(1uz, w - 1)) {
    max = std::max(max, count_energized(grid, {Beam::Direction::S, 0, x}));
    max = std::max(max, count_energized(grid, {Beam::Direction::N, h - 1, x}));
  }
  return max;
}

int main() {
  std::ios::sync_with_stdio(false);

  Grid2D grid;
  std::cin >> grid;

  const auto part1{find_part1(grid)};
  const auto part2{find_part2(grid)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
