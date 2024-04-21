#include "std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Operand {
  enum {
    address,
    literal,
  } type;
  std::size_t index{};
  int value{};
};

struct Instruction {
  enum {
    copy,
    jump_if_not_zero,
    inc,
    dec,
    toggle,
  } type;
  Operand lhs;
  Operand rhs;
};

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
    std::stringstream ls{line};
    if (std::string type; ls >> type) {
      if (type == "cpy") {
        if (Operand src, dst; ls >> src >> dst) {
          ins = {Instruction::copy, src, dst};
          return is;
        }
      }
      if (type == "jnz") {
        if (Operand test, count; ls >> test >> count) {
          ins = {Instruction::jump_if_not_zero, test, count};
          return is;
        }
      }
      if (type == "inc" or type == "dec") {
        if (Operand dst; ls >> dst) {
          ins = {type == "inc" ? Instruction::inc : Instruction::dec, dst};
          return is;
        }
      }
      if (type == "tgl") {
        if (Operand idx; ls >> idx) {
          ins = {Instruction::toggle, idx};
          return is;
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction");
}

auto read(const auto& memory, const Operand& op) {
  if (op.type == Operand::address) {
    return memory.at(op.index);
  } else {
    return op.value;
  }
}

void write(auto& memory, const Operand& op, auto value) {
  if (op.type == Operand::address) {
    memory[op.index] = value;
  }
}

auto multiply(auto& memory, const auto& instructions, const auto pos) {
  if (0 <= pos and pos + 6 < instructions.size()) {
    auto&& is{instructions | views::drop(pos) | views::take(6)};
    using I = Instruction;
    using O = Operand;
    if (is[0].type == I::copy and is[1].type == I::inc and is[2].type == I::dec
        and is[3].type == I::jump_if_not_zero and is[4].type == I::dec
        and is[5].type == I::jump_if_not_zero) {
      if (is[0].lhs.type == O::address and is[0].rhs.type == O::address
          and is[1].lhs.type == O::address and is[2].rhs.type == O::address
          and is[3].lhs.type == O::address and is[3].rhs.type == O::literal
          and is[4].lhs.type == O::address and is[5].lhs.type == O::address
          and is[5].rhs.type == O::literal) {
        const auto& a{is[1].lhs};
        const auto& b{is[0].lhs};
        const auto& c{is[0].rhs};
        const auto& d{is[4].lhs};
        if (is[2].lhs.index == c.index and is[3].lhs.index == c.index and is[3].rhs.value == -2
            and is[5].lhs.index == d.index and is[5].rhs.value == -5) {
          write(memory, a, read(memory, a) + read(memory, d) * read(memory, b));
          return 6;
        }
      }
    }
  }
  return 0;
}

auto execute(auto& memory, auto& instructions, const auto pos) {
  if (const auto jump{multiply(memory, instructions, pos)}) {
    return jump;
  }
  const auto& ins{instructions[pos]};
  switch (ins.type) {
    case Instruction::copy: {
      write(memory, ins.rhs, read(memory, ins.lhs));
    } break;
    case Instruction::jump_if_not_zero: {
      if (read(memory, ins.lhs) != 0) {
        return read(memory, ins.rhs);
      }
    } break;
    case Instruction::inc: {
      write(memory, ins.lhs, read(memory, ins.lhs) + 1);
    } break;
    case Instruction::dec: {
      write(memory, ins.lhs, read(memory, ins.lhs) - 1);
    } break;
    case Instruction::toggle: {
      const auto tgl_pos{pos + read(memory, ins.lhs)};
      if (0 <= tgl_pos and tgl_pos < instructions.size()) {
        auto& ins2{instructions[tgl_pos]};
        switch (ins2.type) {
          case Instruction::copy: {
            ins2.type = Instruction::jump_if_not_zero;
          } break;
          case Instruction::jump_if_not_zero: {
            ins2.type = Instruction::copy;
          } break;
          case Instruction::inc: {
            ins2.type = Instruction::dec;
          } break;
          default: {
            ins2.type = Instruction::inc;
          } break;
        }
      }
    } break;
  }
  return 1;
}

int run(auto instructions, std::array<int, 4> memory) {
  for (std::ptrdiff_t pos{}; 0 <= pos and pos < instructions.size();) {
    const auto jump{execute(memory, instructions, pos)};
    pos += jump;
  }
  return memory[0];
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto instructions{std::views::istream<Instruction>(input) | std::ranges::to<std::vector>()};

  const auto part1{run(instructions, {7, 0, 0, 0})};
  const auto part2{run(instructions, {12, 0, 0, 0})};

  std::print("{} {}\n", part1, part2);

  return 0;
}
