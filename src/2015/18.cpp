#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Light : char {
  on = '#',
  off = '.',
  stuck = 'S',
};

struct Grid {
  std::unordered_map<Vec2, Light> lights;
  int width{};

  [[nodiscard]]
  Grid with_stuck_corners() const {
    Grid g{*this};
    auto n{width - 1};
    for (auto p : {Vec2(0, 0), Vec2(0, n), Vec2(n, 0), Vec2(n, n)}) {
      g.lights.at(p) = Light::stuck;
    }
    return g;
  }

  [[nodiscard]]
  bool is_on(Vec2 p) const {
    return lights.contains(p) and lights.at(p) != Light::off;
  }

  [[nodiscard]]
  int count_adjacent_on(Vec2 pos) const {
    int n_on{};
    for (Vec2 d(-1, -1); d.y() <= 1; d.y() += 1) {
      for (d.x() = -1; d.x() <= 1; d.x() += 1) {
        n_on += int{d != Vec2() and is_on(pos + d)};
      }
    }
    return n_on;
  }

  void step() {
    Grid next{*this};
    for (auto [pos, light] : lights) {
      if (light == Light::stuck) {
        continue;
      }
      const auto on_count{count_adjacent_on(pos)};
      if (is_on(pos) and on_count != 2 and on_count != 3) {
        next.lights.at(pos) = Light::off;
      }
      if (not is_on(pos) and on_count == 3) {
        next.lights.at(pos) = Light::on;
      }
    }
    lights = next.lights;
  }
};

auto search(Grid grid) {
  for (int step{}; step < 100; ++step) {
    grid.step();
  }
  return ranges::count_if(views::values(grid.lights), [](Light l) { return l != Light::off; });
}

Grid parse_grid(std::string_view path) {
  Grid g;
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (g.width == 0) {
      g.width = static_cast<int>(line.size());
    } else if (line.size() != g.width) {
      throw std::runtime_error("every row must be of same width");
    }
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Light::on):
        case std::to_underlying(Light::off):
        case std::to_underlying(Light::stuck): {
          g.lights[p] = Light{ch};
        } break;
        default:
          throw std::runtime_error(std::format("unknown light state {}", ch));
      }
      p.x() += 1;
    }
  }
  if (g.width == 0) {
    throw std::runtime_error("empty input");
  }
  return g;
}

int main() {
  const Grid grid{parse_grid("/dev/stdin")};

  const auto part1{search(grid)};
  const auto part2{search(grid.with_stuck_corners())};

  std::println("{} {}", part1, part2);

  return 0;
}
