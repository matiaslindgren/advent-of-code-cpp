#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& lhs, const auto& rhs) {
  return sum(views::transform(views::zip(lhs, rhs), [](auto&& lr) {
    auto [l, r]{lr};
    return std::abs(l - r);
  }));
}

auto find_part2(const auto& lhs, const auto& rhs) {
  return sum(views::transform(lhs, [&rhs](auto left_id) {
    return left_id * ranges::count(rhs, left_id);
  }));
}

int main() {
  const auto locations{aoc::parse_items<int>("/dev/stdin")};

  // TODO(llvm20) views::stride
  auto lhs{locations | my_std::views::stride(2) | ranges::to<std::vector>()};
  auto rhs{locations | views::drop(1) | my_std::views::stride(2) | ranges::to<std::vector>()};

  ranges::sort(lhs);
  ranges::sort(rhs);

  const auto part1{find_part1(lhs, rhs)};
  const auto part2{find_part2(lhs, rhs)};
  std::println("{} {}", part1, part2);

  return 0;
}
