#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = ndvec::vec3<long double>;
using aoc::skip;
using std::operator""s;

constexpr auto min_coord{200000000000000L};
constexpr auto max_coord{400000000000000L};
constexpr auto distance_epsilon{20.0};

struct Stone {
  Vec3 p;
  Vec3 v;

  [[nodiscard]]
  auto slope() const {
    return v.y() / v.x();
  }

  [[nodiscard]]
  auto intersect() const {
    return p.y() - slope() * p.x();
  }

  [[nodiscard]]
  auto is_past_point(const Vec3& pp) const {
    Vec3 t{v * (pp - p)};
    return t.x() < 0 or t.y() < 0;
  }
};

bool is_inside_square(const Vec3& v, const auto lo, const auto hi) {
  return lo <= std::min(v.x(), v.y()) and std::max(v.x(), v.y()) <= hi;
}

std::optional<Vec3> intersectXY(const Stone& lhs, const Stone& rhs) {
  const auto a{lhs.slope()};
  const auto b{rhs.slope()};
  const auto c{lhs.intersect()};
  const auto d{rhs.intersect()};
  const auto x{(d - c) / (a - b)};
  const auto y{a * x + c};
  if (Vec3 res(x, y, 0); not lhs.is_past_point(res) and not rhs.is_past_point(res)) {
    return res;
  }
  return {};
}

auto find_part1(const auto& stones) {
  auto n{0UZ};
  for (const auto& [i, stone1] : my_std::views::enumerate(stones, 1UZ)) {
    n += ranges::count_if(stones | views::drop(i), [&stone1](const auto& stone2) {
      auto is{intersectXY(stone1, stone2)};
      return is and is_inside_square(*is, min_coord, max_coord);
    });
  }
  return n;
}

auto infer_collision_time(const Vec3& p, const Stone& s) {
  Vec3 t{((p - s.p) / s.v).abs()};
  return std::min(t.x(), t.y());
}

auto find_part2(const auto& stones) {
  for (int radius{}; radius < 4000; ++radius) {
    Vec3 v{};
    const auto lo{-radius};
    const auto hi{radius};
    for (v.y() = lo; v.y() <= hi; ++v.y()) {
      for (v.x() = lo; v.x() <= hi; ++v.x()) {
        if (radius > 0 and is_inside_square(v, lo + 1, hi - 1)) {
          continue;
        }
        v.z() = 0;
        const Stone s1{.p = stones[0].p, .v = stones[0].v - v};
        const Stone s2{.p = stones[1].p, .v = stones[1].v - v};
        if (auto is1{intersectXY(s1, s2)}) {
          if (ranges::all_of(stones | views::drop(2), [&v, &s1, &is1](Stone s3) {
                s3.v = s3.v - v;
                auto is2{intersectXY(s1, s3)};
                return is2 and is1->distance(*is2) < distance_epsilon;
              })) {
            auto t1{infer_collision_time(*is1, s1)};
            auto t2{infer_collision_time(*is1, s2)};
            v.z() = (s1.p.z() - s2.p.z() + t1 * s1.v.z() - t2 * s2.v.z()) / (t1 - t2);
            const Vec3 p_throw(is1->x(), is1->y(), s1.p.z() + t1 * (s1.v.z() - v.z()));
            return std::round(p_throw.sum());
          }
        }
      }
    }
  }
  throw std::runtime_error("search space exhausted");
}

std::istream& operator>>(std::istream& is, Stone& stone) {
  if (std::string line; std::getline(is, line)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    if (Vec3 p, v; ls >> p >> std::ws >> skip("@"s) >> v) {
      stone = {p, v};
    } else {
      throw std::runtime_error(std::format("failed parsing Stone from line '{}'", line));
    }
  }
  return is;
}

int main() {
  const auto stones{aoc::parse_items<Stone>("/dev/stdin")};
  if (stones.size() < 3) {
    throw std::runtime_error("input must contain at least 3 hailstones");
  }

  const auto part1{find_part1(stones)};
  const auto part2{find_part2(stones)};

  std::println("{} {}", part1, part2);

  return 0;
}
