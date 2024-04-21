#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

using Vec2 = aoc::Vec2<int>;
using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Range {
  int lo{}, hi{};
};

struct Area {
  Range x, y;
};

std::istream& operator>>(std::istream& is, Area& area) {
  if (char ch; is >> ch) {
    if (ch == 'x') {
      if (int x; is >> skip("="s) >> x) {
        if (int y0, y1; is >> skip(","s, "y="s) >> y0 >> skip(".."s) >> y1) {
          area = {.x = {x, x + 1}, .y = {y0, y1 + 1}};
        }
      }
    } else if (ch == 'y') {
      if (int y; is >> skip("="s) >> y) {
        if (int x0, x1; is >> skip(","s, "x="s) >> x0 >> skip(".."s) >> x1) {
          area = {.x = {x0, x1 + 1}, .y = {y, y + 1}};
        }
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Area");
}

auto count_water(const auto& areas) {
  const auto min_y{ranges::min(views::transform(areas, [](auto&& a) { return a.y.lo; }))};
  const auto max_y{ranges::max(views::transform(areas, [](auto&& a) { return a.y.hi; }))};
  const auto max_x{ranges::max(views::transform(areas, [](auto&& a) { return a.x.hi; }))};
  const auto index{[=](const Vec2& p) { return p.y() * max_x + p.x(); }};

  std::unordered_set<int> clay, still, flowing;
  const auto contains{[&index](const auto& set, const Vec2& pos) {
    return set.contains(index(pos));
  }};

  for (const auto& area : areas) {
    for (Vec2 p(0, area.y.lo); p.y() < area.y.hi; ++p.y()) {
      for (p.x() = area.x.lo; p.x() < area.x.hi; ++p.x()) {
        clay.insert(index(p));
      }
    }
  }

  Vec2 lhs, rhs;

  for (std::vector q{std::pair{Vec2(0, 1), Vec2(500, 0)}}; not q.empty();) {
    const auto [dir, pos]{q.back()};
    q.pop_back();

    if (contains(clay, pos)) {
      if (dir.x() < 0) {
        lhs = pos;
      } else if (dir.x() > 0) {
        rhs = pos;
      }
      if (lhs.y() > 0 and lhs.y() == rhs.y()) {
        while (++lhs.x() < rhs.x()) {
          still.insert(index(lhs));
        }
        lhs = rhs = Vec2{};
      }
      continue;
    }

    if (contains(still, pos) or pos.y() >= max_y) {
      continue;
    }

    if (dir.y()) {
      if (not contains(flowing, pos)) {
        q.emplace_back(Vec2(-1, 0), pos);
        q.emplace_back(Vec2(1, 0), pos);
        q.emplace_back(dir, pos + dir);
        flowing.insert(index(pos));
      }
    } else {
      const auto below{pos + Vec2(0, 1)};
      if (contains(clay, below) or contains(still, below)) {
        q.emplace_back(dir, pos + dir);
        flowing.insert(index(pos));
      } else {
        q.emplace_back(Vec2(0, 1), pos);
      }
    }
  }

  const auto flowing_size{ranges::count_if(flowing, [&](auto&& i) {
    const auto [y, x]{std::div(i, max_x)};
    return min_y <= y and y <= max_y and not still.contains(i);
  })};

  return std::pair{flowing_size + still.size(), still.size()};
}

int main() {
  const auto areas{aoc::slurp<Area>("/dev/stdin")};
  const auto [part1, part2]{count_water(areas)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
