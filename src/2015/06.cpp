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

int main() {
  constexpr auto size{1000UZ};
  const auto regions{aoc::parse_items<Region>("/dev/stdin")};

  // TODO dry
  std::vector<bool> part1_lights(size * size, false);
  std::vector<long long> part2_lights(size * size, 0);
  for (const auto& r : regions) {
    for (auto y{std::min(r.top, r.bottom)}; y <= std::max(r.top, r.bottom); ++y) {
      for (auto x{std::min(r.left, r.right)}; x <= std::max(r.left, r.right); ++x) {
        const auto current_brightness{part2_lights[y * size + x]};
        switch (r.command) {
          case Region::turn_on: {
            part1_lights[y * size + x] = true;
            part2_lights[y * size + x] = current_brightness + 1;
          } break;
          case Region::turn_off: {
            part1_lights[y * size + x] = false;
            part2_lights[y * size + x] = std::max(0LL, current_brightness - 1);
          } break;
          case Region::toggle: {
            part1_lights[y * size + x] = not part1_lights[y * size + x];
            part2_lights[y * size + x] = current_brightness + 2;
          } break;
        }
      }
    }
  }

  const auto part1{ranges::count_if(part1_lights, std::identity())};
  const auto part2{ranges::fold_left(part2_lights, 0LL, std::plus{})};

  std::println("{} {}", part1, part2);

  return 0;
}
