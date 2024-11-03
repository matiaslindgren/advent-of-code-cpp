#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto rem_euclid(long x, long n) {
  return ((x % n) + n) % n;
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0L, std::plus{})};

// adapted from
// https://www.reddit.com/r/adventofcode/comments/zqezkn/comment/j0xxfet
// accessed 2024-05-19
auto search(std::vector<long> input, long key, int n_iterations) {
  auto index{views::iota(0UZ, input.size()) | ranges::to<std::vector>()};
  while (n_iterations-- > 0) {
    for (auto [i, num] : my_std::views::enumerate(input)) {
      auto src{ranges::find(index, i) - index.begin()};
      auto dst{rem_euclid(src + num * key, ranges::ssize(index) - 1)};
      index.erase(index.begin() + src);
      index.insert(index.begin() + dst, i);
    }
  }
  const auto zero_pos{
      ranges::find(index, (ranges::find(input, 0) - input.begin())) - index.begin()
  };
  return key * sum(views::transform(std::array{1000, 2000, 3000}, [&](int n) {
           return input.at(index.at(rem_euclid(zero_pos + n, ranges::ssize(index))));
         }));
}

int main() {
  const auto input{aoc::parse_items<long>("/dev/stdin")};

  const auto part1{search(input, 1, 1)};
  const auto part2{search(input, 811'589'153, 10)};

  std::println("{} {}", part1, part2);

  return 0;
}
