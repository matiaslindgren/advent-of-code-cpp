#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  garden = '.',
  rock = '#',
  start = 'S',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  int width{};

  [[nodiscard]]
  Tile get(const Vec2& p) const {
    return tiles.contains(p) ? tiles.at(p) : Tile::rock;
  }
};

auto visit_until_limit(const Vec2& start, const Grid& grid, const int limit) {
  std::vector<std::vector<bool>> visited(limit + 1, std::vector<bool>(grid.tiles.size()));
  for (std::deque q{std::pair{0, start}}; not q.empty(); q.pop_front()) {
    if (auto [step, pos]{q.front()}; step <= limit) {
      auto i{pos.y() * grid.width + pos.x()};
      if (not visited.at(step).at(i)) {
        visited.at(step).at(i) = true;
        for (const Vec2& adj : pos.adjacent()) {
          if (grid.get(adj) == Tile::garden) {
            q.emplace_back(step + 1, adj);
          }
        }
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
  const Input l{
      basis(xs[0], xs[1], xs[2]),
      basis(xs[1], xs[0], xs[2]),
      basis(xs[2], xs[0], xs[1]),
  };
  return std::inner_product(l.begin(), l.end(), ys.begin(), 0L);
}

auto search(Vec2 start, Grid grid) {
  constexpr auto diamond_size{65};
  constexpr auto total_steps{26501365};
  constexpr auto repeat_count{total_steps - diamond_size};
  constexpr auto cycle_length{2 * diamond_size + 1};
  static_assert(repeat_count % cycle_length == 0);
  constexpr auto radius{repeat_count / cycle_length};
  static_assert(radius == 202300);

  const auto reachable{visit_until_limit(start, grid, diamond_size + 2 * cycle_length)};
  const auto count_reachable{[&reachable](int steps) {
    return int{steps % 2 == 0} + ranges::count_if(reachable.at(steps), std::identity{});
  }};

  Input xs{};
  Input ys{};
  for (int i{}; i < xs.size(); ++i) {
    xs.at(i) = i;
    ys.at(i) = count_reachable(diamond_size + i * cycle_length);
  }

  return std::pair{
      count_reachable(64),
      lagrange_interpolation(radius, xs, ys),
  };
}

auto repeat_grid(const auto& lines, const int n_repeats) {
  std::vector<std::string> repeated;
  const int center{n_repeats / 2};
  for (int r1{}; r1 < n_repeats; ++r1) {
    for (const std::string& original_line : lines) {
      repeated.emplace_back();
      for (int r2{}; r2 < n_repeats; ++r2) {
        std::string line{original_line};
        if (r1 != center or r2 != center) {
          ranges::replace(line, std::to_underlying(Tile::start), std::to_underlying(Tile::garden));
        }
        repeated.back().append_range(line);
      }
    }
  }
  return repeated;
}

auto parse_input(std::string_view path) {
  Grid g{};
  std::optional<Vec2> start;
  for (Vec2 p; const std::string& line : repeat_grid(aoc::slurp_lines(path), 5)) {
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::garden):
        case std::to_underlying(Tile::rock):
        case std::to_underlying(Tile::start):
          g.tiles[p] = {ch};
          break;
        default:
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      if (g.tiles.at(p) == Tile::start) {
        start = p;
      }
      p.x() += 1;
    }
    if (g.width == 0U) {
      g.width = p.x();
    } else if (g.width != p.x()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.y() += 1;
  }

  if (not start) {
    throw std::runtime_error("grid does not contain a start tile S");
  }
  return std::pair{start.value(), g};
}

int main() {
  const auto [start, grid]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(start, grid)};
  std::println("{} {}", part1, part2);
  return 0;
}
