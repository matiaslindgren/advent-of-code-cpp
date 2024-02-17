import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  garden = '.',
  rock = '#',
  start = 'S',
};

struct Grid2D {
  std::vector<Tile> tiles;
  std::size_t width{};

  std::size_t start_index() const {
    return ranges::find(tiles, Tile::start) - ranges::begin(tiles);
  }

  void append_padding() {
    tiles.append_range(views::repeat(Tile::rock, width));
  }

  std::array<std::size_t, 4> adjacent(const auto i) const {
    return {i - width, i + 1, i + width, i - 1};
  }
};

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::garden):
      case std::to_underlying(Tile::rock):
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

std::istream& operator>>(std::istream& is, Grid2D& grid) {
  Grid2D g;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    line.insert(line.begin(), std::to_underlying(Tile::rock));
    line.insert(line.end(), std::to_underlying(Tile::rock));
    if (not g.width) {
      g.width = line.size();
      g.append_padding();
    } else if (line.size() != g.width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    std::stringstream ls{line};
    g.tiles.append_range(views::istream<Tile>(ls));
  }
  if (g.width and not g.tiles.empty()) {
    g.append_padding();
    grid = g;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Grid2D");
}

Grid2D parse_and_repeat(std::istream& is, const auto repeat_count) {
  std::vector<std::string> lines;
  for (std::string line; std::getline(is, line);) {
    lines.push_back(line);
  }
  std::stringstream input;
  const auto center{repeat_count / 2};
  for (int r1{0}; r1 < repeat_count; ++r1) {
    for (const auto& original_line : lines) {
      for (int r2{0}; r2 < repeat_count; ++r2) {
        auto line{original_line};
        if (not(r1 == center and r2 == center)) {
          ranges::replace(line, std::to_underlying(Tile::start), std::to_underlying(Tile::garden));
        }
        input << line;
      }
      input << "\n";
    }
  }
  Grid2D grid;
  input >> grid;
  return grid;
}

auto visit_until_limit(const Grid2D& grid, const auto limit) {
  const auto n{grid.tiles.size()};
  std::vector<bool> visited((limit + 1) * n);
  for (std::deque q{std::pair{0, grid.start_index()}}; not q.empty(); q.pop_front()) {
    const auto& [step, index] = q.front();
    if (step > limit or visited.at(step * n + index)) {
      continue;
    }
    visited.at(step * n + index) = true;
    for (const auto& adj : grid.adjacent(index)) {
      if (grid.tiles.at(adj) == Tile::garden) {
        q.push_back({step + 1, adj});
      }
    }
  }
  return visited;
}

using Input = std::array<long, 3>;

auto lagrange_interpolation(const long x, const Input& xs, const Input& ys) {
  // https://en.wikipedia.org/wiki/Lagrange_polynomial
  // (2024-01-07)
  const auto basis{[=](const long x0, const long x1, const long x2) {
    return ((x - x1) * (x - x2)) / ((x0 - x1) * (x0 - x2));
  }};
  const Input l = {
      basis(xs[0], xs[1], xs[2]),
      basis(xs[1], xs[0], xs[2]),
      basis(xs[2], xs[0], xs[1]),
  };
  return std::inner_product(l.begin(), l.end(), ys.begin(), 0L);
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const Grid2D grid{parse_and_repeat(input, 5)};

  constexpr auto diamond_size{65};
  constexpr auto total_steps{26501365};
  constexpr auto repeat_count{total_steps - diamond_size};
  constexpr auto cycle_length{2 * diamond_size + 1};
  static_assert(repeat_count % cycle_length == 0);
  constexpr auto radius{repeat_count / cycle_length};
  static_assert(radius == 202300);

  const auto visited{visit_until_limit(grid, diamond_size + 2 * cycle_length)};

  const auto count_reachable{[n = grid.tiles.size(), &visited](const auto steps) {
    const auto include_start{steps % 2 == 0};
    const auto reachable{visited | views::drop(steps * n) | views::take(n)};
    return include_start + ranges::count_if(reachable, std::identity{});
  }};

  const auto part1{count_reachable(64)};

  const Input xs = {0, 1, 2};
  const Input ys = {
      count_reachable(diamond_size + xs[0] * cycle_length),
      count_reachable(diamond_size + xs[1] * cycle_length),
      count_reachable(diamond_size + xs[2] * cycle_length),
  };
  const auto part2{lagrange_interpolation(radius, xs, ys)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
