import std;
import aoc;
import intcode;

auto run(const auto& inputs, const int test_id) {
  using intcode::IntCode;
  using intcode::Mode;

  IntCode ic{.memory = inputs};
  ic.input.push_back(test_id);
  ic.run();
  return ic.output.back();
}

int main() {
  const auto inputs{intcode::parse_input(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run(inputs, 1)};
  const auto part2{run(inputs, 5)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
