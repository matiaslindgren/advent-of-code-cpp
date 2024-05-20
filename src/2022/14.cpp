#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

auto search(auto obstacles) {
  int part1{};
  int part2{};
  for (Vec2 sand(500, 0), max_sand{}, max_rock{ranges::fold_left(obstacles, Vec2{}, ranges::max)};
       not obstacles.contains(sand);
       max_sand = max_sand.max(sand)) {
    if (sand.y() < max_rock.y() + 1) {
      std::optional<Vec2> next;
      for (Vec2 d : {Vec2(0, 1), Vec2(-1, 1), Vec2(1, 1)}) {
        if (Vec2 p{sand + d}; not obstacles.contains(p)) {
          next = p;
          break;
        }
      }
      if (next) {
        sand = *next;
        continue;
      }
    }
    obstacles.insert(sand);
    part1 += int{max_sand.y() < max_rock.y()};
    part2 += 1;
    sand = Vec2(500, 0);
  }
  return std::pair{part1, part2};
}

// TODO (llvm19) ranges::pairwise
auto pairwise(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

auto parse_obstacles(std::string_view path) {
  std::unordered_set<Vec2> obstacles;

  for (std::string line : aoc::slurp_lines(path)) {
    std::vector<Vec2> points;
    {
      ranges::replace(line, ',', ' ');
      std::istringstream ls{" -> "s + line};
      for (Vec2 p; ls >> std::ws >> skip("->"s) >> p;) {
        points.push_back(p);
      }
      if (not ls.eof()) {
        throw std::runtime_error("unknown error while parsing points");
      }
    }
    for (auto [p1, p2] : pairwise(points)) {
      const auto lo{p1.min(p2)};
      const auto hi{p1.max(p2)};
      for (Vec2 p{lo}; p.y() <= hi.y(); ++p.y()) {
        for (p.x() = lo.x(); p.x() <= hi.x(); ++p.x()) {
          obstacles.insert(p);
        }
      }
    }
  }

  if (obstacles.empty()) {
    throw std::runtime_error("empty input");
  }
  return obstacles;
}

int main() {
  const auto obstacles{parse_obstacles("/dev/stdin")};
  const auto [part1, part2]{search(obstacles)};
  std::println("{} {}", part1, part2);
  return 0;
}
