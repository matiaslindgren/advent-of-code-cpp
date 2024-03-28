import std;
import aoc;
import intcode;

using intcode::IntCode;

auto run(const auto& program, const int test_id) {
  IntCode ic(program);
  ic.input.push_back(test_id);
  ic.run_to_end();
  if (ic.output.empty()) {
    throw std::runtime_error("output must not be empty");
  }
  return ic.output.back();
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run(program, 1)};
  const auto part2{run(program, 5)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
