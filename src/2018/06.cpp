#include "std.hpp"
#include "my_std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = aoc::Vec2<int>;
using Points = std::vector<Vec2>;

auto find_grid_corners(const Points& points) {
  constexpr auto intmin{std::numeric_limits<int>::min()};
  constexpr auto intmax{std::numeric_limits<int>::max()};
  return my_std::ranges::fold_left(
      points,
      std::pair{Vec2(intmax, intmax), Vec2(intmin, intmin)},
      [](const auto& corners, const auto& p) {
        auto [tl, br] = corners;
        return std::pair{
            Vec2(std::min(tl.x(), p.x()), std::min(tl.y(), p.y())),
            Vec2(std::max(br.x(), p.x()), std::max(br.y(), p.y()))
        };
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

Points parse_input(std::string path) {
  Points points;
  std::istringstream input{aoc::slurp_file(path)};
  for (Vec2 v; input >> v;) {
    points.push_back(v);
  }
  if (not input.eof()) {
    throw std::runtime_error("input parsing failed");
  }
  return points;
}

int main() {
  const auto points{parse_input("/dev/stdin")};

  const auto [part1, part2]{find_areas(points)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
