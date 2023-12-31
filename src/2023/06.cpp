import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

Ints parse_input(std::istream& is, std::string_view prefix) {
  using aoc::skip;
  if (std::string line; skip(is, prefix) && std::getline(is, line)) {
    std::stringstream ls{line};
    return views::istream<int>(ls) | ranges::to<Ints>();
  }
  return {};
}

constexpr auto count_ways_to_win(auto time, auto dist) {
  return ranges::count_if(views::iota(0, time), [=](auto t_press) {
    return t_press * (time - t_press) > dist;
  });
}

constexpr auto product{
    std::bind(my_std::ranges::fold_left, std::placeholders::_1, 1L, std::multiplies{})
};

constexpr auto find_part1(const Ints& times, const Ints& dists) {
  return product(views::zip(times, dists) | views::transform([](const auto& p) {
                   auto [t, d] = p;
                   return count_ways_to_win(t, d);
                 }));
}

constexpr long pow10_ceil(long x) {
  long p{10};
  for (; x >= 10; x /= 10) {
    p *= 10;
  }
  return p;
}

constexpr auto concat_digits(const Ints& v) {
  return my_std::ranges::fold_left(v, 0L, [](auto res, auto x) { return res * pow10_ceil(x) + x; });
}

constexpr auto find_part2(const Ints& times, const Ints& dists) {
  const auto t{concat_digits(times)};
  const auto d{concat_digits(dists)};
  return count_ways_to_win(t, d);
}

int main() {
  aoc::init_io();
  using std::operator""sv;

  const auto times{parse_input(std::cin, "Time:"sv)};
  const auto dists{parse_input(std::cin, "Distance:"sv)};

  const auto part1{find_part1(times, dists)};
  const auto part2{find_part2(times, dists)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
