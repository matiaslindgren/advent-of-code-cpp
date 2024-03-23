import std;
import aoc;
import intcode;

using std::operator""s;

using intcode::IntCode;

auto run_script(const auto& program, auto&& script) {
  IntCode ic(program);
  for (std::string prompt; prompt != "Input instructions:\n"s;) {
    ic.run_until_output();
    prompt.push_back(static_cast<char>(ic.pop_output().value()));
  }
  for (auto line : script) {
    ic.push_input(std::views::all(line + "\n"s));
  }
  for (;;) {
    ic.run_until_output();
    if (auto out{ic.pop_output().value()}; out > 128) {
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

  std::print("{} {}\n", part1, part2);

  return 0;
}
