#include "std.hpp"

int main() {
  std::ios::sync_with_stdio(false);

  int part1{0};
  int part2{0};
  int i{1};
  for (char ch : std::views::istream<char>(std::cin)) {
    part1 += (ch == '(') - (ch == ')');
    if (part1 < 0 and part2 == 0) {
      part2 = i;
    }
    ++i;
  }
  std::print("{} {}\n", part1, part2);

  return 0;
}
