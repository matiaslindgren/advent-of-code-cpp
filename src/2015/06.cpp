import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Region {
  int top;
  int left;
  int bottom;
  int right;
  enum {
    turn_on,
    turn_off,
    toggle,
  } command;
};

std::istream& operator>>(std::istream& is, Region& r) {
  std::string turn;
  std::string tmp;
  char ch;
  if (((is >> turn && turn == "toggle") || (is >> turn && (turn == "on" || turn == "off")))
      && is >> r.top >> ch && ch == ',' && is >> r.left >> tmp && tmp == "through"
      && is >> r.bottom >> ch && ch == ',' && is >> r.right) {
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
  const auto regions{views::istream<Region>(std::cin) | ranges::to<std::vector<Region>>()};

  // TODO
  std::vector<bool> part1_lights(1'000'000, false);
  std::vector<long long> part2_lights(1'000'000, 0);
  for (const auto& r : regions) {
    for (auto y{std::min(r.top, r.bottom)}; y <= std::max(r.top, r.bottom); ++y) {
      for (auto x{std::min(r.left, r.right)}; x <= std::max(r.left, r.right); ++x) {
        const auto current_brightness{part2_lights[y * 1000 + x]};
        switch (r.command) {
          case Region::turn_on: {
            part1_lights[y * 1000 + x] = true;
            part2_lights[y * 1000 + x] = current_brightness + 1;
          } break;
          case Region::turn_off: {
            part1_lights[y * 1000 + x] = false;
            part2_lights[y * 1000 + x] = std::max(0LL, current_brightness - 1);
          } break;
          case Region::toggle: {
            part1_lights[y * 1000 + x] = !part1_lights[y * 1000 + x];
            part2_lights[y * 1000 + x] = current_brightness + 2;
          } break;
        }
      }
    }
  }

  const auto part1{ranges::count_if(part1_lights, std::identity())};
  const auto part2{my_std::ranges::fold_left(part2_lights, 0LL, std::plus<long long>())};
  std::print("{} {}\n", part1, part2);

  return 0;
}
