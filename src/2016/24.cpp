#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Distance {
  int dst{};
  int len{};
};

struct Grid {
  std::unordered_map<Vec2, int> targets;
  std::unordered_map<Vec2, char> tiles;
  std::size_t width{};

  [[nodiscard]]
  auto bfs(const Vec2 begin) const {
    std::vector<Distance> dist2target;
    std::unordered_set<Vec2> visited;
    for (std::deque q{std::pair{begin, 0}}; not q.empty(); q.pop_front()) {
      auto [pos, len]{q.front()};
      if (not tiles.contains(pos) or tiles.at(pos) == '#') {
        continue;
      }
      if (auto [_, is_new]{visited.insert(pos)}; is_new) {
        if (targets.contains(pos)) {
          dist2target.emplace_back(targets.at(pos), len);
        }
        for (Vec2 adj : pos.adjacent()) {
          q.emplace_back(adj, len + 1);
        }
      }
    }
    return dist2target;
  }
};

auto search_min_paths(const Grid& grid) {
  int part1{std::numeric_limits<int>::max()};
  int part2{std::numeric_limits<int>::max()};
  {
    std::unordered_map<int, std::unordered_map<int, int>> dist;
    for (auto&& [begin, src] : grid.targets) {
      for (auto&& [dst, len] : grid.bfs(begin)) {
        dist[src][dst] = dist[dst][src] = len;
      }
    }
    auto targets{grid.targets | views::values | ranges::to<std::vector<int>>()};
    do {
      const auto begin{targets.front()};
      const auto path{ranges::fold_left(
          targets | views::drop(1),
          Distance{begin, 0},
          [&dist](Distance prev, int dst) {
            auto [src, len]{prev};
            return Distance{dst, len + dist[src][dst]};
          }
      )};
      part1 = std::min(part1, path.len);
      part2 = std::min(part2, path.len + dist[begin][path.dst]);
    } while (ranges::next_permutation(targets).found);
  }
  return std::pair{part1, part2};
}

Grid parse_grid(std::string_view path) {
  Grid g;
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (g.width == 0U) {
      g.width = line.size();
    } else if (g.width != line.size()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7': {
          g.targets[p] = ch - '0';
          g.tiles[p] = '.';
        } break;
        case '.':
        case '#': {
          g.tiles[p] = ch;
        } break;
        default:
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
  }
  return g;
}

int main() {
  const Grid g{parse_grid("/dev/stdin")};
  const auto [part1, part2]{search_min_paths(g)};
  std::println("{} {}", part1, part2);
  return 0;
}
