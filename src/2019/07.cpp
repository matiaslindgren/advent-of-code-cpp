#include "aoc.hpp"
#include "intcode.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using intcode::IntCode;

enum class Mode {
  stateless,
  stateful,
};

auto run(const auto& program, ranges::input_range auto&& phase_set, Mode mode) {
  int max_signal{};
  {
    auto phases{ranges::to<std::vector>(phase_set)};
    do {
      std::vector<IntCode> amps(5);
      for (auto&& [amp, phase] : views::zip(amps, phases)) {
        amp = IntCode(program);
        amp.input.push_back(phase);
      }

      int signal{};

      while (not amps.empty()) {
        ranges::for_each(amps, [&signal](IntCode& amp) {
          amp.input.push_back(signal);
          signal = amp.run_until_output().value_or(signal);
        });
        switch (mode) {
          case Mode::stateless: {
            amps.clear();
          } break;
          case Mode::stateful: {
            const auto rm{ranges::remove_if(amps, [](const IntCode& amp) { return amp.is_done(); })
            };
            amps.erase(rm.begin(), rm.end());
          } break;
        }
      }

      max_signal = std::max(max_signal, signal);
    } while (ranges::next_permutation(phases).found);
  }
  return max_signal;
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{run(program, views::iota(0, 5), Mode::stateless)};
  const auto part2{run(program, views::iota(5, 10), Mode::stateful)};

  std::println("{} {}", part1, part2);

  return 0;
}
