#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using Points = std::vector<Vec2>;

auto find_grid_corners(const Points& points) {
  return ranges::fold_left(
      points,
      std::pair{points.at(0), points.at(0)},
      [](const auto& corners, const Vec2& p) {
        auto&& [tl, br]{corners};
        return std::pair{tl.min(p), br.max(p)};
      }
  );
}

auto find_areas(const Points& points) {
  std::vector<Points> cells(points.size());
  std::unordered_map<Vec2, std::size_t> total_dist;
  const auto [top_left, bottom_right]{find_grid_corners(points)};

  {
    std::unordered_map<Vec2, std::vector<std::size_t>> claims;
    for (Vec2 p(0, top_left.y() - 1); p.y() < bottom_right.y() + 1; ++p.y()) {
      for (p.x() = top_left.x() - 1; p.x() < bottom_right.x() + 1; ++p.x()) {
        const auto& min_dist{ranges::min(views::transform(points, [&](const auto& center) {
          return center.distance(p);
        }))};
        for (auto [id, center] : my_std::views::enumerate(points)) {
          const auto d{center.distance(p)};
          total_dist[p] += d;
          if (d == min_dist) {
            claims[p].push_back(id);
          }
        }
      }
    }
    for (auto [p, ids] : claims) {
      if (ids.size() == 1) {
        cells[ids.front()].push_back(p);
      }
    }
  }

  const auto is_finite{[&](const auto& p) -> bool {
    const auto y_finite{top_left.y() < p.y() and p.y() < bottom_right.y()};
    const auto x_finite{top_left.x() < p.x() and p.x() < bottom_right.x()};
    return y_finite and x_finite;
  }};

  return std::pair{
      ranges::max(cells | views::transform([&](const auto& cell) {
                    return ranges::all_of(cell, is_finite) ? static_cast<int>(cell.size()) : 0;
                  })),
      ranges::count_if(total_dist | views::values, [](auto d) { return d < 10000; })
  };
}

int main() {
  const auto points{aoc::parse_items<Vec2>("/dev/stdin", ',')};
  const auto [part1, part2]{find_areas(points)};
  std::println("{} {}", part1, part2);
  return 0;
}
