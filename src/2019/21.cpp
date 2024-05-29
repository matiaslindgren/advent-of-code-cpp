#include "aoc.hpp"
#include "intcode.hpp"
#include "std.hpp"

using std::operator""s;

using intcode::IntCode;

auto run_script(const auto& program, auto&& script) {
  IntCode ic(program);
  for (std::string prompt; prompt != "Input instructions:\n"s;) {
    auto out{ic.run_until_output().value()};
    prompt.push_back(static_cast<char>(out));
  }
  for (const auto& line : script) {
    ic.input.append_range(std::views::all(line + "\n"s));
  }
  for (;;) {
    if (auto out{ic.run_until_output().value()}; out > 128) {
      return out;
    }
  }
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run_script(
      program,
      std::vector{
          "NOT C T"s,
          "AND D T"s,
          "OR T J"s,
          "NOT A T"s,
          "OR T J"s,
          "WALK"s,
      }
  )};
  const auto part2{run_script(
      program,
      std::vector{
          "NOT B J"s,
          "NOT C T"s,
          "OR T J"s,
          "AND H J"s,
          "NOT A T"s,
          "OR T J"s,
          "AND D J"s,
          "RUN"s,
      }
  )};

  std::println("{} {}", part1, part2);

  return 0;
}
