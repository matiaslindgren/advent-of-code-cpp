#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

struct Grid {
  std::unordered_map<Vec2, int> dots;
  int width{}, height{};

  explicit Grid(int w, int h) : width{w}, height{h} {
  }

  explicit Grid(std::vector<Vec2> points) {
    auto xs{views::transform(points, [](const Vec2& p) { return p.x(); })};
    auto ys{views::transform(points, [](const Vec2& p) { return p.y(); })};
    int x_min{ranges::min(xs)};
    int y_min{ranges::min(ys)};
    int x_max{ranges::max(xs)};
    int y_max{ranges::max(ys)};
    *this = Grid(x_max - x_min + 1, y_max - y_min + 1);
    dots = views::transform(points, [=](Vec2 p) { return std::pair{p - Vec2(x_min, y_min), 1}; })
           | ranges::to<std::unordered_map>();
  }

  void flip(Vec2 axis) {
    Grid g(width, height);
    for (auto [p, dot] : dots) {
      if (axis.x() > 0) {
        g.dots[Vec2(width - p.x() - 1, p.y())] = dot;
      } else {
        g.dots[Vec2(p.x(), height - p.y() - 1)] = dot;
      }
    }
    dots = g.dots;
  }
};

Grid split_and_fold(const Grid& g, Vec2 axis) {
  int w{g.width};
  int h{g.height};
  if (axis.x() > 0) {
    w /= 2;
  } else {
    h /= 2;
  }

  Grid g1(w, h);
  Grid g2(w, h);

  for (auto [p, dot] : g.dots) {
    if ((axis.x() > 0 and p.x() < axis.x()) or (axis.y() > 0 and p.y() < axis.y())) {
      g1.dots[p] = dot;
    }
    if ((axis.x() > 0 and p.x() > axis.x()) or (axis.y() > 0 and p.y() > axis.y())) {
      if (axis.x() > 0) {
        g2.dots[p - Vec2(w + 1, 0)] = dot;
      } else {
        g2.dots[p - Vec2(0, h + 1)] = dot;
      }
    }
  }

  g2.flip(axis);
  for (auto [p, dot] : g2.dots) {
    if (dot > 0) {
      g1.dots[p] = dot;
    }
  }

  return g1;
}

auto find_part1(Grid g, const auto& folds) {
  g = split_and_fold(g, folds.front());
  return ranges::count_if(g.dots | views::values, [](int dot) { return dot > 0; });
}

auto find_part2(Grid g, const auto& folds) {
  for (Vec2 axis : folds) {
    g = split_and_fold(g, axis);
  }
  std::string msg;
  const int letter_width{4};
  for (int begin{}; begin < g.width; begin += letter_width + 1) {
    std::string letter;
    for (Vec2 p; p.y() < g.height; ++p.y()) {
      for (p.x() = begin; p.x() < begin + letter_width; ++p.x()) {
        letter.push_back(g.dots[p] ? '#' : '.');
      }
    }
    msg.push_back(aoc::ocr(letter));
  }
  return msg;
}

auto parse_input(std::string path) {
  auto sections{
      aoc::slurp_file(path) | views::split("\n\n"s)
      | views::transform([](auto&& s) { return ranges::to<std::string>(s); })
      | ranges::to<std::vector>()
  };
  if (sections.size() != 2) {
    throw std::runtime_error("input must contain 2 sections separated by 2 newlines");
  }
  ranges::replace(sections[0], ',', ' ');
  std::istringstream is1{sections[0]};
  std::vector<Vec2> points;
  for (Vec2 p; is1 >> p;) {
    points.push_back(p);
  }
  if (is1.eof()) {
    std::istringstream is2{sections[1]};
    std::vector<Vec2> folds;
    while (is2) {
      char axis;
      int fold;
      if (is2 >> skip("fold along"s) >> axis >> skip("="s) >> fold >> std::ws) {
        if (axis == 'x') {
          folds.push_back(Vec2(fold, -1));
        } else if (axis == 'y') {
          folds.push_back(Vec2(-1, fold));
        } else {
          throw std::runtime_error(std::format("unknown fold axis '{}'", axis));
        }
      }
    }
    if (is2.eof()) {
      return std::pair{Grid(points), folds};
    }
    throw std::runtime_error("failed parsing folds");
  }
  throw std::runtime_error("failed parsing points");
}

int main() {
  const auto [grid, folds]{parse_input("/dev/stdin")};

  const auto part1{find_part1(grid, folds)};
  const auto part2{find_part2(grid, folds)};

  std::println("{} {}", part1, part2);

  return 0;
}
