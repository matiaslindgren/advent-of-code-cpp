#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Step {
  Vec2 direction;
  int count;
};

// TODO (llvm19) ranges::pairwise
auto pairwise(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

auto search(const auto& steps) {
  std::unordered_set<Vec2> head, tail;
  {
    std::array<Vec2, 10> knots{};
    for (Step s : steps) {
      while (s.count-- > 0) {
        knots.at(0) += s.direction;
        for (auto&& [k1, k2] : pairwise(knots)) {
          Vec2 d{k1 - k2};
          if ((d / Vec2(2, 2)).abs().sum() > 0) {
            k2 += d.signum();
          }
        }
        head.insert(knots.at(1));
        tail.insert(knots.at(9));
      }
    }
  }
  return std::pair{head.size(), tail.size()};
}

std::istream& operator>>(std::istream& is, Step& step) {
  if (auto [ch, count]{std::pair(char{}, int{})}; is >> ch >> count) {
    if (count <= 0) {
      throw std::runtime_error("step count must be positive");
    }
    step.count = count;
    switch (ch) {
      case 'U': {
        step.direction = Vec2(0, -1);
      } break;
      case 'R': {
        step.direction = Vec2(1, 0);
      } break;
      case 'D': {
        step.direction = Vec2(0, 1);
      } break;
      case 'L': {
        step.direction = Vec2(-1, 0);
      } break;
      default: {
        throw std::runtime_error(std::format("unknown step '{}', expected [URDL]", ch));
      }
    }
  }
  return is;
}
int main() {
  const auto steps{aoc::slurp<Step>("/dev/stdin")};
  const auto [part1, part2]{search(steps)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
