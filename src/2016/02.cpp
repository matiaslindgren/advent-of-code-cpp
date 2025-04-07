#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using Vec2 = ndvec::vec2<int>;
using Steps = std::vector<Vec2>;

struct Keypad {
  std::vector<std::string> rows;

  [[nodiscard]]
  char get(const Vec2& p) const {
    return rows.at(p.y()).at(p.x());
  }

  [[nodiscard]]
  Vec2 find_key(char key) const {
    for (Vec2 p; p.y() < rows.size(); p.y() += 1) {
      for (p.x() = 0; p.x() < rows.at(p.y()).size(); p.x() += 1) {
        if (get(p) == key) {
          return p;
        }
      }
    }
    throw std::runtime_error(std::format("keypad has no {}", key));
  }
};

std::string find_code(const Keypad& pad, const std::vector<Steps>& instructions) {
  return views::transform(
             instructions,
             [&, p1 = pad.find_key('5')](const Steps& steps) mutable -> char {
               for (const Vec2& step : steps) {
                 if (Vec2 p2{p1 + step}; pad.get(p2) != ' ') {
                   p1 = p2;
                 }
               }
               return pad.get(p1);
             }
         )
         | ranges::to<std::string>();
}

Vec2 parse_step(char ch) {
  switch (ch) {
    case 'U':
      return Vec2(0, -1);
    case 'L':
      return Vec2(-1, 0);
    case 'D':
      return Vec2(0, 1);
    case 'R':
      return Vec2(1, 0);
  }
  throw std::runtime_error(std::format("unknown step '{}'", ch));
}

auto parse_instructions(std::string_view path) {
  return views::transform(
             aoc::slurp_lines(path),
             [](const std::string& line) {
               return views::transform(line, parse_step) | ranges::to<std::vector>();
             }
         )
         | ranges::to<std::vector>();
}

int main() {
  const auto instructions{parse_instructions("/dev/stdin")};

  const Keypad keypad1{
      .rows = {
          "     "s,
          " 123 "s,
          " 456 "s,
          " 789 "s,
          "     "s,
      }
  };
  const auto part1{find_code(keypad1, instructions)};

  const Keypad keypad2{
      .rows = {
          "       "s,
          "   1   "s,
          "  234  "s,
          " 56789 "s,
          "  ABC  "s,
          "   D   "s,
          "       "s,
      }
  };
  const auto part2{find_code(keypad2, instructions)};

  std::println("{} {}", part1, part2);

  return 0;
}
