import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

bool contains_sum_pair(auto&& v, const long sum) {
  return ranges::any_of(my_std::views::cartesian_product(v, v), [&sum](auto&& xy) {
    auto [x, y]{xy};
    return x + y == sum;
  });
}

auto find_subseq_sum_range(auto&& v, const long sum) {
  auto lhs{v.begin()};
  auto rhs{ranges::next(v.begin(), 1, v.end())};
  for (auto s{*lhs}; s != sum and lhs < rhs and rhs != v.end();) {
    if (s < sum) {
      s += *rhs++;
    } else if (s > sum) {
      s -= *lhs++;
    }
  }
  return ranges::subrange(lhs, rhs);
}

auto search(const auto& v) {
  long part1{};
  for (auto [i, x] : my_std::views::enumerate(views::drop(v, 25))) {
    if (not contains_sum_pair(views::take(views::drop(v, i), 25), x)) {
      part1 = x;
      break;
    }
  }
  auto [min, max]{ranges::minmax(find_subseq_sum_range(v, part1))};
  return std::pair{part1, min + max};
}

int main() {
  const auto [part1, part2]{search(aoc::slurp<long>("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
