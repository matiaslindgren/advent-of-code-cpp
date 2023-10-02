import std;

std::string look_and_say(const std::string& init, auto iterations) {
  std::string s{init};
  for (int i{}; i < iterations; ++i) {
    std::ostringstream res;
    for (auto lhs{s.begin()}; lhs != s.end();) {
      auto rhs{lhs};
      int count{};
      for (; rhs != s.end() && *lhs == *rhs; ++rhs) {
        ++count;
      }
      res << count << *lhs;
      lhs = rhs;
    }
    s = res.str();
  }
  return s;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  std::string line;
  std::cin >> line;

  const auto part1{look_and_say(line, 40).length()};
  const auto part2{look_and_say(line, 50).length()};
  std::print("{} {}\n", part1, part2);

  return 0;
}
