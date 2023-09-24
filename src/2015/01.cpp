import std;

int main() {
  using char_reader = std::istream_iterator<std::string::value_type>;
  std::string parens{char_reader{std::cin}, char_reader{}};
  int part1{0};
  int part2{0};
  int pos{0};
  for (auto c : parens) {
    ++pos;
    switch (c) {
    case '(': {
      ++part1;
    } break;
    case ')': {
      --part1;
    } break;
    default:
      throw std::runtime_error{"invalid input"};
    }
    if (part2 == 0 && part1 < 0) {
      part2 = pos;
    }
  }
  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
