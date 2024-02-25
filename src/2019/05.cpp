import std;
import aoc;
import intcode;

using intcode::IntCode;

auto run(const auto& inputs, const int test_id) {
  IntCode ic(inputs);
  ic.push_input(test_id);
  ic.run_to_end();
  return ic.pop_output().value();
}

int main() {
  const auto inputs{intcode::parse_input(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run(inputs, 1)};
  const auto part2{run(inputs, 5)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
