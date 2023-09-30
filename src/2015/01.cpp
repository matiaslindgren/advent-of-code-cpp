import std;

int main() {
  std::ios_base::sync_with_stdio(false);

  int part1{0};
  int part2{0};
  int i{0};
  for (char ch : std::views::istream<char>(std::cin)) {
    int change{(ch == '(') - (ch == ')')};
    ++i;
    if ((part1 += change) < 0 && part2 == 0) {
      part2 = i;
    }
  }
  std::print("{} {}\n", part1, part2);

  return 0;
}
