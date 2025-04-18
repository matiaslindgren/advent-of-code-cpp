#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  safe = '.',
  corrupt = '#',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
};

constexpr int map_width{70};

std::optional<int> shortest_path_length(const Grid& grid) {
  const Vec2 target(map_width, map_width);
  std::unordered_set<Vec2> visited;
  for (std::deque q{std::tuple{Vec2(0, 0), 0}}; not q.empty(); q.pop_front()) {
    auto [pos, n_steps]{q.front()};
    if (pos == target) {
      return n_steps;
    }
    if (not grid.tiles.contains(pos) or grid.tiles.at(pos) == Tile::corrupt) {
      continue;
    }
    if (auto&& [_, is_new]{visited.insert(pos)}; is_new) {
      for (Vec2 adj : pos.adjacent()) {
        q.emplace_back(adj, n_steps + 1);
      }
    }
  }
  return {};
}

auto find_part2(Grid grid, auto&& bytes) {
  for (Vec2 p : bytes) {
    grid.tiles[p] = Tile::corrupt;
    if (not shortest_path_length(grid)) {
      return std::format("{},{}", p.x(), p.y());
    }
  }
  throw std::runtime_error("all bytes are in place but path is still not blocked");
}

auto parse_bytes(std::string_view path) {
  using aoc::skip;
  using std::operator""s;

  std::vector<Vec2> bytes;
  {
    std::istringstream is{aoc::slurp_file(path)};
    for (Vec2 p; is >> std::ws >> p.x() >> skip(","s) >> p.y();) {
      bytes.push_back(p);
    }
  }
  return bytes;
}

Grid build_grid(auto&& bytes) {
  Grid g;
  for (Vec2 p; p.y() <= map_width; ++p.y()) {
    for (p.x() = 0; p.x() <= map_width; ++p.x()) {
      g.tiles[p] = Tile::safe;
    }
  }
  for (Vec2 p : bytes) {
    g.tiles[p] = Tile::corrupt;
  }
  return g;
}

int main() {
  const auto bytes{parse_bytes("/dev/stdin")};

  const Grid grid{build_grid(bytes | views::take(1024))};
  const auto part1{shortest_path_length(grid).value()};
  const auto part2{find_part2(grid, bytes | views::drop(1024))};

  std::println("{} {}", part1, part2);

  return 0;
}
