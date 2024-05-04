#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using aoc::skip;
using std::operator""s;

bool is_inside(int lo, int x, int hi) {
  return lo <= x and x <= hi;
}

bool is_inside(Vec2 lo, Vec2 p, Vec2 hi) {
  return is_inside(lo.x(), p.x(), hi.x()) and is_inside(lo.y(), p.y(), hi.y());
}

bool reaches_target(Vec2 lo, Vec2 hi, Vec2 v) {
  for (Vec2 p; p.x() <= hi.x() and lo.y() <= p.y();) {
    if (is_inside(lo, p, hi)) {
      return true;
    }
    p += v;
    v -= Vec2(v.signum().x(), 1);
  }
  return false;
}

int find_max_y(Vec2 lo, Vec2 hi, int v_0) {
  for (int y{}, y_max{}, v_y{v_0}; y >= lo.y();) {
    if (is_inside(lo.y(), y, hi.y())) {
      return y_max;
    }
    y += v_y;
    y_max = std::max(y_max, y);
    v_y -= 1;
  }
  return std::numeric_limits<int>::min();
}

auto find_part1(Vec2 lo, Vec2 hi) {
  return ranges::max(views::iota(lo.y(), lo.abs().y()) | views::transform([=](int v_y) {
                       return find_max_y(lo, hi, v_y);
                     }));
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part2(Vec2 lo, Vec2 hi) {
  return sum(
      my_std::views::cartesian_product(
          views::iota(0, hi.x() + 1),
          views::iota(lo.y(), lo.abs().y())
      )
      | views::transform([=](auto&& v_xy) {
          auto [v_x, v_y]{v_xy};
          return reaches_target(lo, hi, Vec2(v_x, v_y));
        })
  );
}

std::pair<Vec2, Vec2> parse_target_area(std::istream& is) {
  if (Vec2 lo, hi; is >> skip("target area: x="s) >> lo.x() >> skip(".."s) >> hi.x()
                   >> skip(", y="s) >> lo.y() >> skip(".."s) >> hi.y() >> std::ws) {
    if (is.eof()) {
      return std::pair{lo, hi};
    }
  }
  throw std::runtime_error("failed parsing target area");
}

int main() {
  std::ios::sync_with_stdio(false);
  const auto [lo, hi]{parse_target_area(std::cin)};

  const auto part1{find_part1(lo, hi)};
  const auto part2{find_part2(lo, hi)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
