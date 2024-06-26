#include "aoc.hpp"
#include "intcode.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using intcode::IntCode;

struct NAT {
  long x{};
  long y{};
  long dst{};
};

struct NIC {
  IntCode ic;
  bool idle{};

  void push(auto... inputs) {
    (ic.input.push_back(inputs), ...);
  }
};

std::optional<NAT> run_step(auto& nats, NIC& nic) {
  if (nic.ic.input.empty()) {
    nic.push(-1);
    nic.idle = true;
  } else {
    nic.idle = false;
  }
  nic.ic.run_while_input();
  if (nic.ic.output.empty()) {
    return {};
  }
  auto dst{nic.ic.pop_output().value()};
  auto x{nic.ic.pop_output().value()};
  auto y{nic.ic.pop_output().value()};
  if (0 <= dst and dst < 50) {
    return NAT{x, y, dst};
  }
  if (dst == 255) {
    nats.back() = {x, y, dst};
    return {};
  }
  throw std::runtime_error(std::format("bad dst {}", dst));
}

auto run(auto nics) {
  for (std::vector<NAT> nats{{}};;) {
    for (NIC& nic : nics) {
      if (auto dst_nat{run_step(nats, nic)}) {
        nics.at(dst_nat->dst).push(dst_nat->x, dst_nat->y);
      }
    }
    if (ranges::all_of(nics, &NIC::idle)) {
      auto nat{nats.rbegin()};
      nics[0].push(nat->x, nat->y);
      if (nat->dst) {
        if (nats.size() > 1 and nat->y == (nat + 1)->y) {
          return std::pair{nats.front().y, nats.back().y};
        }
        nats.emplace_back();
      }
    }
  }
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};
  const auto nics{
      views::iota(0, 50) | views::transform([&program](auto i) {
        NIC nic{.ic = IntCode(program), .idle = false};
        nic.push(i);
        return nic;
      })
      | ranges::to<std::vector>()
  };
  const auto [part1, part2]{run(nics)};
  std::println("{} {}", part1, part2);
  return 0;
}
