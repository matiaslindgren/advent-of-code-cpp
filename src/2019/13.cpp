import std;
import aoc;
import intcode;

using intcode::IntCode;
using Vec2 = aoc::Vec2<int>;

auto find_part1(const auto& program) {
  int n_blocks{};
  for (IntCode ic(program); not ic.is_done();) {
    ic.run_until_output();
    ic.pop_output();
    ic.run_until_output();
    ic.pop_output();
    ic.run_until_output();
    n_blocks += (ic.pop_output().value_or(0) == 2);
  }
  return n_blocks;
}

auto find_part2(const auto& program) {
  int score{};
  {
    IntCode ic(program);
    ic.store(0, 2, intcode::Mode::address);
    for (std::optional<Vec2> paddle; not ic.is_done();) {
      ic.run_until_output();
      auto x{ic.pop_output()};
      ic.run_until_output();
      auto y{ic.pop_output()};
      ic.run_until_output();
      auto t{ic.pop_output()};

      if (not x or not y or not t) {
        break;
      }

      Vec2 pos(*x, *y);
      auto tile{*t};

      if (pos == Vec2(-1, 0)) {
        score = tile;
      } else if (tile == 3) {
        paddle = pos;
      } else if (tile == 4 and not paddle) {
        ic.push_input(0);
      } else if (tile == 4) {
        ic.push_input((pos - *paddle).signum().x());
      }
    }
  }
  return score;
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{find_part1(program)};
  const auto part2{find_part2(program)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
