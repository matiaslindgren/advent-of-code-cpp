#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

int main() {
  long part1{};
  long part2{};
  for (auto&& line : aoc::slurp_lines("/dev/stdin")) {
    std::istringstream ls{line};
    if (int l{}, w{}, h{}; ls >> l >> skip("x"s) >> w >> skip("x"s) >> h) {
      int surface{2 * (l * w + w * h + h * l)};
      int slack{std::min({l * w, w * h, h * l})};
      int bow{2 * std::min({l + w, w + h, h + l})};
      int ribbon{l * w * h};
      part1 += surface + slack;
      part2 += ribbon + bow;
    } else {
      throw std::runtime_error("failed parsing present");
    }
  }
  std::println("{} {}", part1, part2);
  return 0;
}
