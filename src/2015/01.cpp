import std;
import aoc;

int main() {
  aoc::init_io();

  int part1{0};
  int part2{0};
  int i{1};
  for (char ch : std::views::istream<char>(std::cin)) {
    part1 += (ch == '(') - (ch == ')');
    if (part1 < 0 && part2 == 0) {
      part2 = i;
    }
    ++i;
  }
  std::print("{} {}\n", part1, part2);

  return 0;
}
