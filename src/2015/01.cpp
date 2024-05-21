#include "aoc.hpp"
#include "std.hpp"

int main() {
  int part1{};
  int part2{};
  for (int i{1}; char ch : aoc::slurp_file("/dev/stdin")) {
    part1 += int{ch == '('} - int{ch == ')'};
    if (part1 < 0 and part2 == 0) {
      part2 = i;
    }
    ++i;
  }
  std::println("{} {}", part1, part2);
  return 0;
}
