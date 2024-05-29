#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = ndvec::vec3<int>;

int count_3d_surface(const auto& points) {
  int area{};

  const int lo{ranges::min(views::transform(points, [](Vec3 p) { return p.min() - 1; }))};
  const int hi{ranges::max(views::transform(points, [](Vec3 p) { return p.max() + 1; }))};
  std::unordered_set<Vec3> visited;
  for (std::deque q{Vec3()}; not q.empty(); q.pop_front()) {
    Vec3 p{q.front()};
    if (lo <= p.min() and p.max() <= hi) {
      if (auto [_, is_new]{visited.insert(p)}; is_new) {
        for (Vec3 adj : p.adjacent()) {
          if (points.contains(adj)) {
            area += 1;
          } else {
            q.push_back(adj);
          }
        }
      }
    }
  }

  return area;
}

int main() {
  int n_sides{};
  std::unordered_set<Vec3> points;
  for (Vec3 p : aoc::parse_items<Vec3>("/dev/stdin", ',')) {
    n_sides += 6;
    for (Vec3 adj : p.adjacent()) {
      if (points.contains(adj)) {
        n_sides -= 2;
      }
    }
    points.insert(p);
  }

  const auto part1{n_sides};
  const auto part2{count_3d_surface(points)};

  std::println("{} {}", part1, part2);

  return 0;
}
