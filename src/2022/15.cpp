#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<long>;

struct Sensor {
  Vec2 pos;
  Vec2 beacon;
  long range{};

  bool in_range(Vec2 p) const {
    return pos.distance(p) <= range;
  }
};

constexpr long limit1{2'000'000};
constexpr long limit2{4'000'000};

auto find_part1(const auto& sensors) {
  const auto x_min{
      ranges::min(views::transform(sensors, [](const Sensor& s) { return s.pos.x() - s.range; }))
  };
  const auto x_max{
      ranges::max(views::transform(sensors, [](const Sensor& s) { return s.pos.x() + s.range; }))
  };
  return ranges::count_if(views::iota(x_min, x_max + 1), [&sensors](long x) {
    return ranges::any_of(sensors, [p = Vec2(x, limit1)](const Sensor& s) {
      return s.in_range(p) and p != s.beacon;
    });
  });
}

auto find_part2(const auto& sensors) {
  // adapted from
  // https://www.reddit.com/r/adventofcode/comments/zmcn64/comment/j0b90nr
  // accessed 2024-05-11
  std::unordered_set<long> a, b;
  for (const Sensor& s : sensors) {
    a.insert(s.pos.y() - s.pos.x() + s.range + 1);
    a.insert(s.pos.y() - s.pos.x() - s.range - 1);
    b.insert(s.pos.y() + s.pos.x() + s.range + 1);
    b.insert(s.pos.y() + s.pos.x() - s.range - 1);
  }
  const auto points{
      my_std::views::cartesian_product(a, b) | views::transform([](auto&& ab) {
        auto [a, b]{ab};
        return Vec2((b - a) / 2, (a + b) / 2);
      })
      | ranges::to<std::vector>()
  };
  if (auto match{ranges::find_if(
          points,
          [&sensors](Vec2 p) {
            return 0 < p.min() and p.max() < limit2
                   and not ranges::any_of(sensors, [p](const Sensor& s) { return s.in_range(p); });
          }
      )};
      match != points.end()) {
    return limit2 * match->x() + match->y();
  }
  throw std::runtime_error("search space exhausted, no match");
}

std::istream& operator>>(std::istream& is, Sensor& sensor) {
  if (Vec2 pos;
      is >> std::ws >> skip("Sensor at x="s) >> pos.x() >> skip(", y="s) >> pos.y() >> skip(":"s)) {
    if (Vec2 beacon; is >> std::ws >> skip("closest beacon is at x="s) >> beacon.x()
                     >> skip(", y="s) >> beacon.y()) {
      sensor = Sensor{pos, beacon, pos.distance(beacon)};
    }
  }
  if (is.fail() and not is.eof()) {
    throw std::runtime_error("failed parsing Sensor");
  }
  return is;
}

int main() {
  const auto sensors{aoc::parse_items<Sensor>("/dev/stdin")};

  const auto part1{find_part1(sensors)};
  const auto part2{find_part2(sensors)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
