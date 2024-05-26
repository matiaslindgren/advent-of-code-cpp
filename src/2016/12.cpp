#include "aoc.hpp"
#include "std.hpp"

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

auto execute(auto& memory, const Instruction& ins) {
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
  }
  return 1;
}

int run(const auto& instructions, std::array<int, 4> memory) {
  for (std::ptrdiff_t pos{}; 0 <= pos and pos < instructions.size();) {
    const auto jump{execute(memory, instructions[pos])};
    pos += jump;
  }
  return memory.at(0);
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
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line '{}'", line));
    }
  }
  return is;
}

int main() {
  const auto instructions{aoc::parse_items<Instruction>("/dev/stdin")};

  const auto part1{run(instructions, {0, 0, 0, 0})};
  const auto part2{run(instructions, {0, 0, 1, 0})};

  std::println("{} {}", part1, part2);

  return 0;
}
