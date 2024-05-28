#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

inline constexpr auto count_ways_to_win{[](auto time, auto dist) {
  return ranges::count_if(views::iota(0, time), [=](auto t_press) {
    return t_press * (time - t_press) > dist;
  });
}};

constexpr auto product{std::__bind_back(ranges::fold_left, 1L, std::multiplies{})};

auto find_part1(const Ints& times, const Ints& dists) {
  return product(views::zip(times, dists) | views::transform(my_std::apply_fn(count_ways_to_win)));
}

long pow10_ceil(long x) {
  long p{10};
  for (; x >= 10; x /= 10) {
    p *= 10;
  }
  return p;
}

auto concat_digits(const Ints& v) {
  return ranges::fold_left(v, 0L, [](auto res, auto x) { return res * pow10_ceil(x) + x; });
}

auto find_part2(const Ints& times, const Ints& dists) {
  const auto t{concat_digits(times)};
  const auto d{concat_digits(dists)};
  return count_ways_to_win(t, d);
}

Ints parse_input(std::istream& is, std::string&& prefix) {
  using aoc::skip;
  if (std::string line; is >> skip(std::move(prefix)) and std::getline(is, line)) {
    std::istringstream ls{line};
    return views::istream<int>(ls) | ranges::to<Ints>();
  }
  return {};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  using std::operator""s;

  const auto times{parse_input(input, "Time:"s)};
  const auto dists{parse_input(input, "Distance:"s)};

  const auto part1{find_part1(times, dists)};
  const auto part2{find_part2(times, dists)};

  std::println("{} {}", part1, part2);

  return 0;
}
