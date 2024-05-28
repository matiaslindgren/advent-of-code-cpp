#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using aoc::skip;
using std::operator""s;

struct Range {
  int lo{};
  int hi{};
};

struct Area {
  Range x;
  Range y;
};

void fill(auto& clay, const auto& areas) {
  ranges::for_each(areas, [&clay](const Area& a) {
    for (Vec2 p(0, a.y.lo); p.y() < a.y.hi; ++p.y()) {
      for (p.x() = a.x.lo; p.x() < a.x.hi; ++p.x()) {
        clay.insert(p);
      }
    }
  });
}

void fill_horizontal(auto& set, Vec2 lhs, Vec2 rhs) {
  while (++lhs.x() < rhs.x()) {
    set.insert(lhs);
  }
}

auto count_water(const auto& areas) {
  const auto min_y{ranges::min(views::transform(areas, [](auto&& a) { return a.y.lo; }))};
  const auto max_y{ranges::max(views::transform(areas, [](auto&& a) { return a.y.hi; }))};

  std::unordered_set<Vec2> clay;
  std::unordered_set<Vec2> still;
  std::unordered_set<Vec2> flowing;

  fill(clay, areas);

  Vec2 lhs;
  Vec2 rhs;

  for (std::vector q{std::pair{Vec2(0, 1), Vec2(500, 0)}}; not q.empty();) {
    const auto [dir, pos]{q.back()};
    q.pop_back();

    if (clay.contains(pos)) {
      if (dir.x() < 0) {
        lhs = pos;
      } else if (dir.x() > 0) {
        rhs = pos;
      }
      if (lhs.y() > 0 and lhs.y() == rhs.y()) {
        fill_horizontal(still, lhs, rhs);
        lhs = Vec2{};
        rhs = Vec2{};
      }
      continue;
    }

    if (still.contains(pos) or pos.y() >= max_y) {
      continue;
    }

    if (dir.y() != 0 and not flowing.contains(pos)) {
      q.emplace_back(Vec2(-1, 0), pos);
      q.emplace_back(Vec2(1, 0), pos);
      q.emplace_back(dir, pos + dir);
      flowing.insert(pos);
    } else if (dir.y() == 0) {
      const auto below{pos + Vec2(0, 1)};
      if (clay.contains(below) or still.contains(below)) {
        q.emplace_back(dir, pos + dir);
        flowing.insert(pos);
      } else {
        q.emplace_back(Vec2(0, 1), pos);
      }
    }
  }

  const auto flowing_count{ranges::count_if(flowing, [&](const Vec2& p) {
    return min_y <= p.y() and p.y() <= max_y and not still.contains(p);
  })};

  return std::pair{flowing_count + still.size(), still.size()};
}

std::istream& operator>>(std::istream& is, Area& area) {
  if (char ch{}; is >> ch) {
    if (ch == 'x') {
      if (int x{}; is >> skip("="s) >> x) {
        if (int y0{}, y1{}; is >> skip(","s, "y="s) >> y0 >> skip(".."s) >> y1) {
          area = {.x = {x, x + 1}, .y = {y0, y1 + 1}};
        }
      }
    } else if (ch == 'y') {
      if (int y{}; is >> skip("="s) >> y) {
        if (int x0{}, x1{}; is >> skip(","s, "x="s) >> x0 >> skip(".."s) >> x1) {
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

int main() {
  const auto areas{aoc::parse_items<Area>("/dev/stdin")};
  const auto [part1, part2]{count_water(areas)};
  std::println("{} {}", part1, part2);
  return 0;
}
