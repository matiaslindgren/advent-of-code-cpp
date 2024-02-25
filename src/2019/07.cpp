import std;
import aoc;
import intcode;

namespace ranges = std::ranges;
namespace views = std::views;

using intcode::IntCode;

auto run(const auto& program, ranges::input_range auto&& phase_set, const bool stateful) {
  int max_signal{};
  {
    auto phases{ranges::to<std::vector>(phase_set)};
    do {
      std::vector<IntCode> amps(5);
      for (auto&& [amp, phase] : views::zip(amps, phases)) {
        amp = IntCode(program);
        amp.push_input(phase);
      }

      int signal{};

      while (not amps.empty()) {
        ranges::for_each(amps, [&signal](IntCode& amp) {
          amp.push_input(signal);
          amp.run_until_output();
          signal = amp.pop_output().value_or(signal);
        });
        if (not stateful) {
          amps.clear();
        } else {
          const auto rm{ranges::remove_if(amps, [](const IntCode& amp) { return amp.is_done(); })};
          amps.erase(rm.begin(), rm.end());
        }
      }

      max_signal = std::max(max_signal, signal);
    } while (ranges::next_permutation(phases).found);
  }
  return max_signal;
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run(program, views::iota(0, 5), false)};
  const auto part2{run(program, views::iota(5, 10), true)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
