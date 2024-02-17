import std;

namespace ranges = std::ranges;
namespace views = std::views;

auto search_for_recipes(const int target, const auto& digits) {
  std::string part1{};
  std::ptrdiff_t part2{};
  {
    std::vector<int> r{3, 7};
    auto i1{0uz}, i2{1uz};
    while (part1.empty() or part2 == 0) {
      const auto x1{r[i1]};
      const auto x2{r[i2]};
      const auto [d1, d2]{std::div(x1 + x2, 10)};
      if (d1) {
        r.push_back(d1);
      }
      r.push_back(d2);
      i1 = (i1 + 1 + x1) % r.size();
      i2 = (i2 + 1 + x2) % r.size();
      if (part1.empty()) {
        if (r.size() >= target + 10) {
          // clang-format off
          part1 = (
            views::drop(r, target)
            | views::take(10)
            | views::transform([](const auto& x) { return '0' + x; })
            | ranges::to<std::string>()
          );
          // clang-format on
        }
      }
      if (part2 == 0) {
        const auto tail{views::drop(r, r.size() - digits.size() - 1)};
        if (const auto match{ranges::search(tail, digits)}; not match.empty()) {
          part2 = match.begin() - r.begin();
        }
      }
    }
  }
  return std::pair{part1, part2};
}

int main() {
  std::ios::sync_with_stdio(false);

  int target;
  std::vector<int> digits;
  {
    std::string input;
    std::cin >> input;
    std::istringstream{input} >> target;
    digits = views::transform(input, [](char ch) { return ch - '0'; }) | ranges::to<std::vector>();
  }

  const auto [part1, part2]{search_for_recipes(target, digits)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
