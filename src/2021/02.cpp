#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

struct Step {
  enum {
    forward,
    down,
    up,
  } direction{};
  int len{};
};

std::istream& operator>>(std::istream& is, Step& step) {
  if (std::string dir; is >> dir) {
    if (int len; is >> len) {
      if (dir == "forward"s) {
        step = {Step::forward, len};
      } else if (dir == "down"s) {
        step = {Step::down, len};
      } else if (dir == "up"s) {
        step = {Step::up, len};
      } else {
        throw std::runtime_error(std::format("unknown direction {}", dir));
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing step");
}

auto search(const auto& steps) {
  int h{};
  int d{};
  int a{};
  for (const Step& s : steps) {
    switch (s.direction) {
      case Step::forward: {
        h += s.len;
        d += a * s.len;
      } break;
      case Step::down: {
        a += s.len;
      } break;
      case Step::up: {
        a -= s.len;
      } break;
    }
  }
  return std::pair{h * a, h * d};
}

int main() {
  const auto steps{aoc::parse_items<Step>("/dev/stdin")};
  const auto [part1, part2]{search(steps)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
