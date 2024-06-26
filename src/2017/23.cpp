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
  enum struct Type : unsigned char {
    set,
    sub,
    mul,
    jnz,
  } type{};
  Operand lhs;
  Operand rhs;
};

int find_part1(const auto& instructions) {
  const auto i0{instructions[0]};
  if (i0.type != Instruction::Type::set) {
    throw std::runtime_error("unexpected program, cannot compute part1");
  }
  const auto b{i0.rhs.value};
  return (b - 2) * (b - 2);
}

int find_part2(const auto& instructions) {
  const auto i0{instructions[0]};
  const auto i4{instructions[4]};
  const auto i5{instructions[5]};
  const auto i7{instructions[7]};
  const auto i30{instructions[30]};

  using Type = Instruction::Type;
  if (not(i0.type == Type::set and i4.type == Type::mul and i5.type == Type::sub
          and i7.type == Type::sub and i30.type == Type::sub)) {
    throw std::runtime_error("unexpected program, cannot compute part2");
  }

  const auto b{i0.rhs.value * i4.rhs.value - i5.rhs.value};
  const auto c{b - i7.rhs.value};
  const auto step{-i30.rhs.value};

  int not_prime_count{};

  for (int x{b}; x <= c; x += step) {
    bool is_prime{true};
    for (int d{2}; is_prime and d * d < x; ++d) {
      is_prime = (x % d != 0);
    }
    not_prime_count += not is_prime;
  }

  return not_prime_count;
}

std::istream& operator>>(std::istream& is, Operand& op) {
  if (std::string s; is >> s and not s.empty()) {
    if (const char reg_ch{s.front()}; 'a' <= reg_ch and reg_ch <= 'h') {
      op = {.type = Operand::address, .index = static_cast<unsigned>(reg_ch - 'a')};
    } else {
      op = {.type = Operand::literal, .value = std::stoi(s)};
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction::Type& type) {
  if (std::string s; is >> s) {
    using Type = Instruction::Type;
    if (s == "set") {
      type = Type::set;
    } else if (s == "sub") {
      type = Type::sub;
    } else if (s == "mul") {
      type = Type::mul;
    } else if (s == "jnz") {
      type = Type::jnz;
    } else {
      throw std::runtime_error(std::format("unknown instruction '{}'", s));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (Instruction::Type type{}; is >> type) {
    if (Operand idx, val; is >> idx >> val) {
      ins = {type, idx, val};
    }
  }
  return is;
}

int main() {
  const auto instructions{aoc::parse_items<Instruction>("/dev/stdin")};

  const auto part1{find_part1(instructions)};
  const auto part2{find_part2(instructions)};

  std::println("{} {}", part1, part2);

  return 0;
}
