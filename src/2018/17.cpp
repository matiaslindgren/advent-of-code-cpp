import std;
import aoc;
import my_std;

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
      if (int x; skip(is, "="s) && is >> x) {
        if (int y0, y1; skip(is, ","s, "y="s) && is >> y0 && skip(is, ".."s) && is >> y1) {
          area = {.x = {x, x + 1}, .y = {y0, y1 + 1}};
        }
      }
    } else if (ch == 'y') {
      if (int y; skip(is, "="s) && is >> y) {
        if (int x0, x1; skip(is, ","s, "x="s) && is >> x0 && skip(is, ".."s) && is >> x1) {
          area = {.x = {x0, x1 + 1}, .y = {y, y + 1}};
        }
      }
    }
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Area");
}

struct Vec2 {
  int y{}, x{};

  Vec2 operator+(const Vec2& rhs) const {
    return {y + rhs.y, x + rhs.x};
  }
};

constexpr auto min{
    std::__bind_back(my_std::ranges::fold_left, std::numeric_limits<int>::max(), ranges::min)
};
constexpr auto max{
    std::__bind_back(my_std::ranges::fold_left, std::numeric_limits<int>::min(), ranges::max)
};

auto count_water(const auto& areas) {
  const auto min_y{min(views::transform(areas, [](auto&& a) { return a.y.lo; }))};
  const auto max_y{max(views::transform(areas, [](auto&& a) { return a.y.hi; }))};
  const auto max_x{max(views::transform(areas, [](auto&& a) { return a.x.hi; }))};
  const auto index{[=](const Vec2& p) { return p.y * max_x + p.x; }};

  std::unordered_set<int> clay, still, flowing;
  const auto contains{[&index](const auto& set, const Vec2& pos) {
    return set.contains(index(pos));
  }};

  for (const auto& area : areas) {
    for (Vec2 p{.y = area.y.lo}; p.y < area.y.hi; ++p.y) {
      for (p.x = area.x.lo; p.x < area.x.hi; ++p.x) {
        clay.insert(index(p));
      }
    }
  }

  Vec2 lhs, rhs;

  for (std::vector q{std::pair{Vec2{.y = 1}, Vec2{.y = 0, .x = 500}}}; !q.empty();) {
    const auto [dir, pos]{q.back()};
    q.pop_back();

    if (contains(clay, pos)) {
      if (dir.x < 0) {
        lhs = pos;
      } else if (dir.x > 0) {
        rhs = pos;
      }
      if (lhs.y > 0 && lhs.y == rhs.y) {
        while (++lhs.x < rhs.x) {
          still.insert(index(lhs));
        }
        lhs = rhs = Vec2{};
      }
      continue;
    }

    if (contains(still, pos) || pos.y >= max_y) {
      continue;
    }

    if (dir.y) {
      if (!contains(flowing, pos)) {
        q.emplace_back(Vec2{.x = -1}, pos);
        q.emplace_back(Vec2{.x = 1}, pos);
        q.emplace_back(dir, pos + dir);
        flowing.insert(index(pos));
      }
    } else {
      const auto below{pos + Vec2{.y = 1}};
      if (contains(clay, below) || contains(still, below)) {
        q.emplace_back(dir, pos + dir);
        flowing.insert(index(pos));
      } else {
        q.emplace_back(Vec2{.y = 1}, pos);
      }
    }
  }

  const auto flowing_size{ranges::count_if(flowing, [&](auto&& i) {
    const auto [y, x]{std::div(i, max_x)};
    return min_y <= y && y <= max_y && !still.contains(i);
  })};

  return std::pair{flowing_size + still.size(), still.size()};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto areas{views::istream<Area>(input) | ranges::to<std::vector>()};

  const auto [part1, part2]{count_water(areas)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
