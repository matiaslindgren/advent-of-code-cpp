#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto search_for_recipes(const int target, const auto& digits) {
  std::string part1{};
  std::ptrdiff_t part2{};
  {
    std::vector<int> r{3, 7};
    auto i1{0UL};
    auto i2{1UL};
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

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int target{}; is >> target and target > 0) {
    std::vector<int> digits;
    for (auto x{target}; x > 0;) {
      auto [p, q]{std::div(x, 10)};
      digits.insert(digits.begin(), q);
      x = p;
    }
    return std::pair{target, digits};
  }
  throw std::runtime_error("input should be a single positive integer");
}

int main() {
  const auto [target, digits]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search_for_recipes(target, digits)};
  std::println("{} {}", part1, part2);
  return 0;
}
