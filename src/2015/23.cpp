#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::is_alpha;

enum Code : unsigned char {
  half,
  triple,
  increment,
  jump,
  jump_if_even,
  jump_if_one,
};

struct Instruction {
  Code code{};
  int addr{};
  int jmp{};
};

using Memory = std::array<int, 8>;
using Program = std::vector<Instruction>;

void run(Memory& memory, const Program& program) {
  for (int i{}; 0 <= i and i < program.size();) {
    const Instruction ins{program[i]};
    int jump{1};
    switch (ins.code) {
      case Code::half: {
        memory.at(ins.addr) /= 2;
      } break;
      case Code::triple: {
        memory.at(ins.addr) *= 3;
      } break;
      case Code::increment: {
        memory.at(ins.addr) += 1;
      } break;
      case Code::jump: {
        jump = ins.jmp;
      } break;
      case Code::jump_if_even: {
        if (memory.at(ins.addr) % 2 == 0) {
          jump = ins.jmp;
        }
      } break;
      case Code::jump_if_one: {
        if (memory.at(ins.addr) == 1) {
          jump = ins.jmp;
        }
      } break;
    }
    i += jump;
  }
}

std::istream& operator>>(std::istream& is, Code& code) {
  if (std::string op{}; is >> op) {
    if (op == "hlf") {
      code = Code::half;
    } else if (op == "tpl") {
      code = Code::triple;
    } else if (op == "inc") {
      code = Code::increment;
    } else if (op == "jmp") {
      code = Code::jump;
    } else if (op == "jie") {
      code = Code::jump_if_even;
    } else if (op == "jio") {
      code = Code::jump_if_one;
    } else {
      throw std::runtime_error(std::format("unknown opcode '{}'", op));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (Code code{}; is >> code) {
    int jmp{};
    if (code == Code::jump and is >> jmp) {
      ins = {code, -1, jmp};
      return is;
    }
    std::string addr;
    if ((is >> addr and not addr.empty() and is_alpha(addr.front()) and code != Code::jump_if_one
         and code != Code::jump_if_even)
        or is >> jmp) {
      ins = {code, addr.front() - 'a', jmp};
      return is;
    }
  }
  return is;
}

int main() {
  const auto program{aoc::parse_items<Instruction>("/dev/stdin")};

  Memory memory;
  memory.fill(0);

  run(memory, program);
  const auto part1{memory[1]};

  memory.fill(0);
  memory[0] = 1;
  run(memory, program);
  const auto part2{memory[1]};

  std::println("{} {}", part1, part2);

  return 0;
}
