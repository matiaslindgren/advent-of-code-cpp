import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto min_coord{200000000000000L};
constexpr auto max_coord{400000000000000L};
constexpr auto distance_epsilon{20.0};

struct Vec3 {
  long double x{}, y{}, z{};

  Vec3 operator-(const Vec3& v) const {
    return {x - v.x, y - v.y, z - v.z};
  }

  Vec3 operator-(const auto a) const {
    return {x - a, y - a, z - a};
  }

  auto sum() const {
    return x + y + z;
  }

  auto l1_distance(const Vec3& v) const {
    return std::abs(x - v.x) + std::abs(y - v.y) + std::abs(z - v.z);
  }
};

struct Stone {
  Vec3 p{};
  Vec3 v{};

  auto slope() const {
    return v.y / v.x;
  }

  auto intersect() const {
    return p.y - slope() * p.x;
  }

  auto is_past_point(const Vec3& pp) const {
    return v.x * (pp.x - p.x) < 0 || v.y * (pp.y - p.y) < 0;
  }
};

std::istream& operator>>(std::istream& is, Vec3& v) {
  if (long double x, y, z; is >> x && skip(is, ","s) && is >> y && skip(is, ","s) && is >> z) {
    v = {x, y, z};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Vec3");
}

std::istream& operator>>(std::istream& is, Stone& stone) {
  if (Vec3 p, v; is >> p >> std::ws && skip(is, "@"s) && is >> v) {
    stone = {p, v};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Stone");
}

bool is_inside_square(const Vec3& v, const auto lo, const auto hi) {
  return lo <= std::min(v.x, v.y) && std::max(v.x, v.y) <= hi;
}

std::optional<Vec3> intersectXY(const Stone& lhs, const Stone& rhs) {
  const auto a{lhs.slope()};
  const auto b{rhs.slope()};
  const auto c{lhs.intersect()};
  const auto d{rhs.intersect()};
  const auto x{(d - c) / (a - b)};
  const auto y{a * x + c};
  if (const Vec3 res{x, y}; !lhs.is_past_point(res) && !rhs.is_past_point(res)) {
    return res;
  }
  return {};
}

auto find_part1(const auto& stones) {
  auto n{0uz};
  for (const auto& [i, stone1] : my_std::views::enumerate(stones, 1uz)) {
    n += ranges::count_if(stones | views::drop(i), [&stone1](const auto& stone2) {
      const auto is{intersectXY(stone1, stone2)};
      return is && is_inside_square(*is, min_coord, max_coord);
    });
  }
  return n;
}

auto infer_collision_time(const Vec3& p, const Stone& s) {
  const auto tx{(p.x - s.p.x) / s.v.x};
  const auto ty{(p.y - s.p.y) / s.v.y};
  return std::min(std::abs(tx), std::abs(ty));
}

auto find_part2(const auto& stones) {
  for (int radius{0}; radius < 4000; ++radius) {
    Vec3 v{};
    const auto lo{-radius};
    const auto hi{radius};
    for (v.y = lo; v.y <= hi; ++v.y) {
      for (v.x = lo; v.x <= hi; ++v.x) {
        if (radius > 0 && is_inside_square(v, lo + 1, hi - 1)) {
          continue;
        }
        v.z = 0;
        const Stone s1{.p = stones[0].p, .v = stones[0].v - v};
        const Stone s2{.p = stones[1].p, .v = stones[1].v - v};
        if (const auto is1{intersectXY(s1, s2)}) {
          if (ranges::all_of(stones | views::drop(2), [&v, &s1, &is1](Stone s3) {
                s3.v = s3.v - v;
                const auto is2{intersectXY(s1, s3)};
                return is2 && is1->l1_distance(*is2) < distance_epsilon;
              })) {
            const auto t1{infer_collision_time(*is1, s1)};
            const auto t2{infer_collision_time(*is1, s2)};
            v.z = (s1.p.z - s2.p.z + t1 * s1.v.z - t2 * s2.v.z) / (t1 - t2);
            const Vec3 p_throw{
                .x = is1->x,
                .y = is1->y,
                .z = s1.p.z + t1 * (s1.v.z - v.z),
            };
            return std::round(p_throw.sum());
          }
        }
      }
    }
  }
  throw std::runtime_error("search space exhausted");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto stones{views::istream<Stone>(input) | ranges::to<std::vector>()};
  if (stones.size() < 3) {
    throw std::runtime_error("too few hailstones in input");
  }

  const auto part1{find_part1(stones)};
  const auto part2{find_part2(stones)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
