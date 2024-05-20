#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Instruction {
  enum {
    acc,
    jmp,
    nop,
  } type;
  int value;
};

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (std::string type; is >> type) {
    if (int value; is >> value) {
      if (type == "acc") {
        ins = {Instruction::acc, value};
      } else if (type == "jmp") {
        ins = {Instruction::jmp, value};
      } else if (type == "nop") {
        ins = {Instruction::nop, value};
      } else {
        throw std::runtime_error(std::format("unknown instruction '{}'", type));
      }
    }
  }
  return is;
}

auto run(auto program, const int mutate = -1) {
  int acc{}, i{};
  for (std::unordered_set<int> seen; not seen.contains(i) and i < program.size(); ++i) {
    seen.insert(i);
    if (i == mutate) {
      auto& ins{program.at(i)};
      switch (ins.type) {
        case Instruction::nop: {
          ins.type = Instruction::jmp;
        } break;
        case Instruction::jmp: {
          ins.type = Instruction::nop;
        } break;
        default: {
        } break;
      }
    }
    {
      const auto& ins{program.at(i)};
      switch (ins.type) {
        case Instruction::acc: {
          acc += ins.value;
        } break;
        case Instruction::jmp: {
          i += ins.value - 1;
        } break;
        default: {
        } break;
      }
    }
  }
  return std::pair{acc, i >= program.size()};
}

auto find_part1(const auto& program) {
  return run(program).first;
}

auto find_part2(const auto& program) {
  int res{};
  for (auto [mutate, ins] : my_std::views::enumerate(program)) {
    if (ins.type == Instruction::nop or ins.type == Instruction::jmp) {
      if (auto [acc, finished]{run(program, mutate)}; finished) {
        res = acc;
      }
    }
  }
  return res;
}

int main() {
  const auto program{aoc::parse_items<Instruction>("/dev/stdin")};

  const auto part1{find_part1(program)};
  const auto part2{find_part2(program)};

  std::println("{} {}", part1, part2);

  return 0;
}
