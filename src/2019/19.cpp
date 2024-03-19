import std;
import aoc;
import intcode;
import my_std;

namespace views = std::views;

using intcode::IntCode;

inline constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0L, std::plus{})};

auto search(const auto& program) {
  const auto is_pulled{[&program](int y, int x) {
    IntCode ic(program);
    ic.push_input(x);
    ic.push_input(y);
    ic.run_until_output();
    return ic.pop_output().value();
  }};

  auto part1{sum(views::transform(views::iota(0, 50 * 50), [&is_pulled](auto i) {
    auto [y, x]{std::div(i, 50)};
    return is_pulled(y, x);
  }))};

  int y{}, x{};
  while (not is_pulled(y, x + 99)) {
    ++y;
    while (not is_pulled(y + 99, x)) {
      ++x;
    }
  }

  return std::pair{part1, x * 10'000 + y};
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};
  const auto [part1, part2]{search(program)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
