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

struct count_ways_to_win_fn {
  auto operator()(auto time, auto dist) const {
    return ranges::count_if(views::iota(0, time), [=](auto t_press) {
      return t_press * (time - t_press) > dist;
    });
  }
};
inline auto count_ways_to_win = count_ways_to_win_fn{};

constexpr auto product{std::__bind_back(my_std::ranges::fold_left, 1L, std::multiplies{})};

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
  return my_std::ranges::fold_left(v, 0L, [](auto res, auto x) { return res * pow10_ceil(x) + x; });
}

auto find_part2(const Ints& times, const Ints& dists) {
  const auto t{concat_digits(times)};
  const auto d{concat_digits(dists)};
  return count_ways_to_win(t, d);
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  using std::operator""sv;

  const auto times{parse_input(input, "Time:"sv)};
  const auto dists{parse_input(input, "Distance:"sv)};

  const auto part1{find_part1(times, dists)};
  const auto part2{find_part2(times, dists)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
