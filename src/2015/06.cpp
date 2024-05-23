#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

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

std::istream& operator>>(std::istream& is, Region& r) {
  std::string turn;
  std::string tmp;
  char ch{};
  if (((is >> turn and turn == "toggle") or (is >> turn and (turn == "on" or turn == "off")))
      and is >> r.top >> ch and ch == ',' and is >> r.left >> tmp and tmp == "through"
      and is >> r.bottom >> ch and ch == ',' and is >> r.right) {
    if (turn == "toggle") {
      r.command = Region::toggle;
    } else if (turn == "on") {
      r.command = Region::turn_on;
    } else {
      r.command = Region::turn_off;
    }
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Region");
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

int main() {
  constexpr auto size{1000UZ};
  const auto regions{aoc::parse_items<Region>("/dev/stdin")};

  std::vector<std::pair<bool, int>> lights(size * size);
  for (const auto& r : regions) {
    for (auto y{std::min(r.top, r.bottom)}; y <= std::max(r.top, r.bottom); ++y) {
      for (auto x{std::min(r.left, r.right)}; x <= std::max(r.left, r.right); ++x) {
        auto& light{lights[y * size + x]};
        switch (r.command) {
          case Region::turn_on: {
            light.first = true;
            light.second += 1;
          } break;
          case Region::turn_off: {
            light.first = false;
            light.second = std::max(0, light.second - 1);
          } break;
          case Region::toggle: {
            light.first = not light.first;
            light.second += 2;
          } break;
        }
      }
    }
  }

  const auto part1{sum(lights | views::elements<0>)};
  const auto part2{sum(lights | views::elements<1>)};

  std::println("{} {}", part1, part2);

  return 0;
}
