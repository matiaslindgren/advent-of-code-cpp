#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = ndvec::vec3<long>;
using aoc::skip;
using std::operator""s;

struct Cuboid {
  Vec3 lo;
  Vec3 hi;

  [[nodiscard]]
  bool is_init() const {
    return std::max(lo.abs().max(), hi.abs().max()) <= 50;
  }

  [[nodiscard]]
  std::optional<Cuboid> intersection(const Cuboid& other) const {
    Vec3 lower{lo.max(other.lo)};
    Vec3 upper{hi.min(other.hi)};
    if (lower.min(upper) == lower) {
      return Cuboid{lower, upper};
    }
    return {};
  }

  [[nodiscard]]
  long volume() const {
    return ((lo - hi).abs() + Vec3(1, 1, 1)).prod();
  }
};

struct Step {
  bool on{};
  Cuboid cuboid;
};

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

long reboot(ranges::range auto&& steps) {
  std::vector<std::pair<long, Cuboid>> intersections;
  for (Step s : steps) {
    Cuboid c1{s.cuboid};
    for (auto [sign, c2] : intersections | ranges::to<std::vector>()) {
      if (auto is{c1.intersection(c2)}) {
        // inclusion-exclusion principle
        intersections.emplace_back(-1 * sign, *is);
      }
    }
    intersections.emplace_back(int{s.on}, c1);
  }
  return sum(views::transform(intersections, [](auto&& is) {
    auto [sign, cuboid]{is};
    return sign * cuboid.volume();
  }));
}

std::istream& operator>>(std::istream& is, Step& step) {
  if (std::string state; is >> std::ws >> state) {
    if (state == "on"s) {
      step.on = true;
    } else if (state == "off"s) {
      step.on = false;
    } else {
      throw std::runtime_error(std::format("state should be 'on' or 'off', not '{}'", state));
    }
    if (Cuboid c; is >> std::ws >> skip("x="s) >> c.lo.x() >> skip(".."s) >> c.hi.x()
                  >> skip(",y="s) >> c.lo.y() >> skip(".."s) >> c.hi.y() >> skip(",z="s) >> c.lo.z()
                  >> skip(".."s) >> c.hi.z()) {
      step.cuboid = c;
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Step");
}

int main() {
  const auto steps{aoc::parse_items<Step>("/dev/stdin")};

  const auto part1{reboot(steps | views::filter([](auto s) { return s.cuboid.is_init(); }))};
  const auto part2{reboot(steps)};

  std::println("{} {}", part1, part2);

  return 0;
}
