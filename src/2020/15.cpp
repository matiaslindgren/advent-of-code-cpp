import std;
import my_std;
import aoc;

auto search(const auto& items, const int n) {
  std::unordered_map<int, int> age1, age2;
  for (auto [t, x] : my_std::views::enumerate(items, 1)) {
    age1[x] = age2[x] = t;
  }
  int t0{items.back()};
  for (auto t1{items.size() + 1}; t1 <= n; ++t1) {
    t0 = age2[t0] - age1[t0];
    if ((age1[t0] = std::exchange(age2[t0], t1)) == 0) {
      age1[t0] = t1;
    }
  }
  return t0;
}

auto parse_input(std::string_view path) {
  std::ios::sync_with_stdio(false);
  using aoc::skip;
  using std::operator""s;

  std::vector<int> items;
  for (int x; std::cin >> x;) {
    items.push_back(x);
    std::cin >> skip(","s);
  }
  return items;
}

int main() {
  const auto items{parse_input("/dev/stdin")};

  const auto part1{search(items, 2020)};
  const auto part2{search(items, 30'000'000)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
