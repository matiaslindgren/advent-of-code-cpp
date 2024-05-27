#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Operand {
  enum : unsigned char {
    address,
    literal,
  } type{};
  std::size_t index{};
  int value{};
};

struct Instruction {
  enum : unsigned char {
    copy,
    jump_if_not_zero,
    add,
    out,
  } type{};
  Operand lhs;
  Operand rhs;
};

auto read(const auto& memory, const Operand& op) {
  if (op.type == Operand::address) {
    return memory.at(op.index);
  }
  return op.value;
}

void write(auto& memory, const Operand& op, auto value) {
  if (op.type == Operand::address) {
    memory.at(op.index) = value;
  } else {
    throw std::runtime_error("cannot write to a literal");
  }
}

auto execute(auto& memory, const Instruction& ins, auto& out) {
  switch (ins.type) {
    case Instruction::copy: {
      write(memory, ins.lhs, read(memory, ins.rhs));
    } break;
    case Instruction::jump_if_not_zero: {
      if (read(memory, ins.lhs) != 0) {
        return read(memory, ins.rhs);
      }
    } break;
    case Instruction::add: {
      write(memory, ins.lhs, read(memory, ins.lhs) + read(memory, ins.rhs));
    } break;
    case Instruction::out: {
      out.push_back(read(memory, ins.lhs));
    } break;
  }
  return 1;
}

int run(const auto& instructions) {
  for (int a{};; ++a) {
    std::array<int, 4> memory{a, 0, 0, 0};
    std::vector<int> out;
    for (std::ptrdiff_t pos{}; 0 <= pos and pos < instructions.size();) {
      const auto jump{execute(memory, instructions[pos], out)};
      if (not out.empty()) {
        const auto back{out.end() - 1};
        if (*back != 0 and *back != 1) {
          break;
        }
        if (out.size() > 1 and *(back - 1) == *back) {
          break;
        }
        if (out.size() > 1000) {
          return a;
        }
      }
      pos += jump;
    }
  }
}

std::istream& operator>>(std::istream& is, Operand& op) {
  if (std::string s; is >> s and not s.empty()) {
    if (const char reg_ch{s.front()}; 'a' <= reg_ch and reg_ch <= 'd') {
      op = {.type = Operand::address, .index = static_cast<unsigned>(reg_ch - 'a')};
    } else {
      op = {.type = Operand::literal, .value = std::stoi(s)};
    }
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Operand");
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (std::string type; ls >> type) {
      if (Operand src, dst; type == "cpy" and ls >> src >> dst) {
        ins = {Instruction::copy, dst, src};
      } else if (Operand test, count; type == "jnz" and ls >> test >> count) {
        ins = {Instruction::jump_if_not_zero, test, count};
      } else if (Operand dst; (type == "inc" or type == "dec") and ls >> dst) {
        ins = {
            Instruction::add,
            dst,
            {.type = Operand::literal, .value = type == "dec" ? -1 : 1},
        };
      } else if (Operand dst; type == "out" and ls >> dst) {
        ins = {Instruction::out, dst};
      } else {
        throw std::runtime_error(
            std::format("unknown instruction '{}', failed parsing line {}", type, line)
        );
      }
    }
  }
  return is;
}

int main() {
  const auto instructions{aoc::parse_items<Instruction>("/dev/stdin")};
  std::println("{}", run(instructions));
  return 0;
}
