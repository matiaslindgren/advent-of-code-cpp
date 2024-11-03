#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

// TODO (llvm19) ranges::adjacent
auto window2(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}
auto window3(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2));
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& depths) {
  return sum(window2(depths) | views::transform(my_std::apply_fn(std::less{})));
}

auto find_part2(const auto& depths) {
  return find_part1(
      window3(depths) | views::transform(my_std::apply_fn([](auto... xs) { return (... + xs); }))
      | ranges::to<std::vector>()
  );
}

int main() {
  const auto depths{aoc::parse_items<int>("/dev/stdin")};
  const auto part1{find_part1(depths)};
  const auto part2{find_part2(depths)};
  std::println("{} {}", part1, part2);
  return 0;
}
