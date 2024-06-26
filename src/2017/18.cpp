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
  long value{};
};

struct Instruction {
  enum struct Type : unsigned char {
    add,
    jump_if_gt_zero,
    modulo,
    multiply,
    receive,
    set,
    send,
  } type{};
  Operand lhs;
  Operand rhs;
};

struct Program {
  long loc{};
  long sent_count{};
  std::array<long, ('p' - 'a') + 1> memory{};
  std::deque<long> pipe;
  bool blocked{false};

  Program(const int id) {
    memory.fill(0);
    memory['p' - 'a'] = id;
  }

  [[nodiscard]]
  auto read(const Operand& op) const {
    if (op.type == Operand::address) {
      return memory.at(op.index);
    }
    return op.value;
  }

  void write(const Operand& op, auto value) {
    if (op.type == Operand::address) {
      memory.at(op.index) = value;
      return;
    }
    throw std::runtime_error("cannot write to a literal");
  }

  void binop_update(const Instruction& ins, auto&& f) {
    write(ins.lhs, f(read(ins.lhs), read(ins.rhs)));
  }

  void execute(const Instruction& ins, auto& out) {
    if (blocked and pipe.empty()) {
      return;
    }

    auto jump{1L};

    using Type = Instruction::Type;
    switch (ins.type) {
      case Type::set: {
        write(ins.lhs, read(ins.rhs));
      } break;
      case Type::add: {
        binop_update(ins, std::plus{});
      } break;
      case Type::modulo: {
        binop_update(ins, std::modulus{});
      } break;
      case Type::multiply: {
        binop_update(ins, std::multiplies{});
      } break;
      case Type::jump_if_gt_zero: {
        if (read(ins.lhs) > 0) {
          jump = read(ins.rhs);
        }
      } break;
      case Type::receive: {
        if ((blocked = pipe.empty())) {
          return;
        }
        write(ins.lhs, pipe.front());
        pipe.pop_front();
      } break;
      case Type::send: {
        out.push_back(read(ins.lhs));
        sent_count += 1;
      } break;
    }

    loc += jump;
  }
};

auto run_part1(const auto& instructions) {
  Program p0(0);
  Program p1(1);
  while (not p0.blocked) {
    p0.execute(instructions[p0.loc], p1.pipe);
  }
  return p1.pipe.back();
}

auto run_part2(const auto& instructions) {
  Program p0(0);
  Program p1(1);
  while (not(p0.blocked and p1.blocked)) {
    p0.execute(instructions[p0.loc], p1.pipe);
    p1.execute(instructions[p1.loc], p0.pipe);
  }
  return p1.sent_count;
}

std::istream& operator>>(std::istream& is, Instruction::Type& t) {
  if (std::string type; is >> type and not type.empty()) {
    using Type = Instruction::Type;
    if (type == "add") {
      t = Type::add;
    } else if (type == "jgz") {
      t = Type::jump_if_gt_zero;
    } else if (type == "mod") {
      t = Type::modulo;
    } else if (type == "mul") {
      t = Type::multiply;
    } else if (type == "rcv") {
      t = Type::receive;
    } else if (type == "set") {
      t = Type::set;
    } else if (type == "snd") {
      t = Type::send;
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing instruction type");
}

std::istream& operator>>(std::istream& is, Operand& op) {
  if (std::string s; is >> s and not s.empty()) {
    if (const char reg_ch{s.front()}; 'a' <= reg_ch and reg_ch <= 'z') {
      op = {.type = Operand::address, .index = static_cast<unsigned>(reg_ch - 'a')};
    } else {
      op = {.type = Operand::literal, .value = std::stoi(s)};
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    using Type = Instruction::Type;
    if (Type type{}; ls >> type) {
      if (Operand idx; (type == Type::send or type == Type::receive) and ls >> idx) {
        ins = {type, idx};
      } else if (Operand idx, value; ls >> idx >> value) {
        ins = {type, idx, value};
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing instruction from line '{}'", line));
    }
  }
  return is;
}

int main() {
  const auto instructions{aoc::parse_items<Instruction>("/dev/stdin")};

  const auto part1{run_part1(instructions)};
  const auto part2{run_part2(instructions)};

  std::println("{} {}", part1, part2);

  return 0;
}
