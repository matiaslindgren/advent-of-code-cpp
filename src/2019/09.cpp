#include "std.hpp"
#include "aoc.hpp"
#include "intcode.hpp"

using intcode::IntCode;

auto run(const auto& program, const int input) {
  IntCode ic(program);
  ic.input.push_back(input);
  return ic.run_until_output().value();
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run(program, 1)};
  const auto part2{run(program, 2)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
