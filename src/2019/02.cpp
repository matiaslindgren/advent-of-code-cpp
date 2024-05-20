#include "aoc.hpp"
#include "intcode.hpp"
#include "std.hpp"

auto run(const auto& program, const int noun, const int verb) {
  using intcode::IntCode;
  using intcode::Mode;

  IntCode ic(program);
  ic.store(1, noun, Mode::address);
  ic.store(2, verb, Mode::address);
  ic.run_to_end();
  return ic.load(0, Mode::address);
}

auto find_part1(const auto& program) {
  return run(program, 12, 2);
}

auto find_part2(const auto& program) {
  for (int noun{}; noun < 100; ++noun) {
    for (int verb{}; verb < 100; ++verb) {
      if (run(program, noun, verb) == 1969'07'20) {
        return 100 * noun + verb;
      }
    }
  }
  throw std::runtime_error("search space exhausted during part2");
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{find_part1(program)};
  const auto part2{find_part2(program)};

  std::println("{} {}", part1, part2);

  return 0;
}
