#include "aoc.hpp"
#include "intcode.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using intcode::IntCode;
using Vec2 = ndvec::vec2<IntCode::Int>;

auto find_part1(const auto& program) {
  int n_blocks{};
  for (IntCode ic(program); not ic.is_done();) {
    ic.run_until_output();
    ic.run_until_output();
    n_blocks += (ic.run_until_output().value_or(0) == 2);
  }
  return n_blocks;
}

auto find_part2(const auto& program) {
  IntCode::Int score{};

  IntCode ic(program);
  ic.store(0, 2, intcode::Mode::address);
  for (std::optional<Vec2> paddle; not ic.is_done();) {
    auto x{ic.run_until_output()};
    auto y{ic.run_until_output()};
    auto t{ic.run_until_output()};
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
      ic.input.push_back(0);
    } else if (tile == 4) {
      ic.input.push_back((pos - *paddle).signum().x());
    }
  }

  return score;
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{find_part1(program)};
  const auto part2{find_part2(program)};

  std::println("{} {}", part1, part2);

  return 0;
}
