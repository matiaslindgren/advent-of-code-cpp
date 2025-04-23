#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  wall = '#',
  track = '.',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;

  [[nodiscard]]
  Tile get(const Vec2& p) const {
    return tiles.contains(p) ? tiles.at(p) : Tile::wall;
  }
};

auto find_all_distances(const Grid& grid, Vec2 start, Vec2 end) {
  std::unordered_map<Vec2, int> dist;
  for (std::deque q{std::tuple{start, 0}}; not q.empty(); q.pop_front()) {
    auto [pos, n_steps]{q.front()};
    dist[pos] = n_steps;
    if (pos == end) {
      return dist;
    }
    for (Vec2 adj : pos.adjacent()) {
      if (grid.get(adj) == Tile::track and not dist.contains(adj)) {
        q.emplace_back(adj, n_steps + 1);
      }
    }
  }
  throw std::runtime_error(
      std::format("search space exhausted, could not find path from {} to {}", start, end)
  );
}

auto solve(Grid grid, Vec2 start, Vec2 end) {
  int part1{};
  int part2{};

  const auto distances{find_all_distances(grid, start, end)};
  auto track_nodes{distances | views::keys | ranges::to<std::vector<Vec2>>()};

  for (auto [i, p1] : my_std::views::enumerate(track_nodes)) {
    for (auto p2 : views::drop(track_nodes, i + 1)) {
      const auto d1{distances.at(p1)};
      const auto d2{distances.at(p2)};
      const auto l1{p1.distance(p2)};
      if (std::max(d1, d2) - std::min(d1, d2) - l1 >= 100) {
        part1 += l1 <= 2;
        part2 += l1 <= 20;
      }
    }
  }

  return std::pair{part1, part2};
}

auto parse_grid(std::string_view path) {
  Grid grid{};
  Vec2 start, end;
  {
    Vec2 p;
    std::istringstream is{aoc::slurp_file(path)};
    std::size_t width{};

    for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
      if (width == 0UZ) {
        width = line.size();
      } else if (width != line.size()) {
        throw std::runtime_error("every row must be of equal length");
      }

      for (p.x() = 0; char ch : line) {
        switch (ch) {
          case 'S': {
            start = p;
            grid.tiles[p] = Tile::track;
          } break;
          case 'E': {
            end = p;
            grid.tiles[p] = Tile::track;
          } break;
          case std::to_underlying(Tile::wall):
          case std::to_underlying(Tile::track): {
            grid.tiles[p] = {ch};
          } break;
          default:
            throw std::runtime_error(std::format("unknown tile '{}'", ch));
        }
        p.x() += 1;
      }
    }

    if (p == Vec2()) {
      throw std::runtime_error("empty input");
    }
  }
  return std::tuple{grid, start, end};
}

int main() {
  const auto [grid, start, end]{parse_grid("/dev/stdin")};
  const auto [part1, part2]{solve(grid, start, end)};
  std::println("{} {}", part1, part2);
  return 0;
}
