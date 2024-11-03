#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Region {
  int top;
  int left;
  int bottom;
  int right;
  enum : unsigned char {
    turn_on,
    turn_off,
    toggle,
  } command;
};

auto simulate(const auto& regions) {
  constexpr auto size{1000UZ};
  std::vector<std::pair<bool, int>> lights(size * size);
  for (const auto& r : regions) {
    for (auto y{std::min(r.top, r.bottom)}; y <= std::max(r.top, r.bottom); ++y) {
      for (auto x{std::min(r.left, r.right)}; x <= std::max(r.left, r.right); ++x) {
        auto&& [on, bright]{lights[y * size + x]};
        switch (r.command) {
          case Region::turn_on: {
            on = true;
            bright += 1;
          } break;
          case Region::turn_off: {
            on = false;
            bright = std::max(0, bright - 1);
          } break;
          case Region::toggle: {
            on = not on;
            bright += 2;
          } break;
        }
      }
    }
  }
  return lights;
}

std::istream& operator>>(std::istream& is, Region& r) {
  if (std::string turn;
      ((is >> turn and turn == "toggle") or (is >> turn and (turn == "on" or turn == "off")))
      and is >> r.top >> skip(","s) >> r.left >> std::ws >> skip("through"s) >> r.bottom
              >> skip(","s) >> r.right) {
    if (turn == "toggle") {
      r.command = Region::toggle;
    } else if (turn == "on") {
      r.command = Region::turn_on;
    } else {
      r.command = Region::turn_off;
    }
  } else {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

int main() {
  const auto regions{aoc::parse_items<Region>("/dev/stdin")};

  const auto lights{simulate(regions)};
  const auto part1{sum(lights | views::elements<0>)};
  const auto part2{sum(lights | views::elements<1>)};

  std::println("{} {}", part1, part2);

  return 0;
}
