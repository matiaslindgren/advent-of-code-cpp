import std;
import aoc;
import intcode;

auto run(const auto& inputs, const int noun, const int verb) {
  using intcode::IntCode;
  using intcode::Mode;

  IntCode ic(inputs);
  ic.store(1, noun, Mode::address);
  ic.store(2, verb, Mode::address);
  ic.run();
  return ic.load(0, Mode::address);
}

auto find_part1(const auto& inputs) {
  return run(inputs, 12, 2);
}

auto find_part2(const auto& inputs) {
  for (int noun{}; noun < 100; ++noun) {
    for (int verb{}; verb < 100; ++verb) {
      if (run(inputs, noun, verb) == 1969'07'20) {
        return 100 * noun + verb;
      }
    }
  }
  throw std::runtime_error("search space exhausted during part2");
}

int main() {
  const auto inputs{intcode::parse_input(aoc::slurp_file("/dev/stdin"))};

  const auto part1{find_part1(inputs)};
  const auto part2{find_part2(inputs)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
