import std;

int main() {
  int part1{0};
  int part2{0};
  int i{0};
  for (char ch : std::views::istream<char>(std::cin)) {
    ++i;
    if ((part1 += (ch == '(') - (ch == ')')) < 0 && part2 == 0) {
      part2 = i;
    }
  }
  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
