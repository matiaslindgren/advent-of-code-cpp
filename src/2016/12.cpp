import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Register {
  std::size_t index;
};
struct Literal {
  int value;
};
using Operand = std::variant<Register, Literal>;

struct Add {
  Operand dst;
  Operand src;
};
struct Copy {
  Operand dst;
  Operand src;
};
struct JumpIfNotZero {
  Operand test;
  Operand count;
};
using Instruction = std::variant<Add, Copy, JumpIfNotZero>;

std::istream& operator>>(std::istream& is, Operand& op) {
  if (std::string s; is >> s && !s.empty()) {
    if (const char reg_ch{s.front()}; 'a' <= reg_ch && reg_ch <= 'd') {
      op = Register{static_cast<unsigned>(reg_ch - 'a')};
      return is;
    }
    op = Literal{std::stoi(s)};
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
          ins = Copy{dst, src};
          return is;
        }
      }
      if (type == "jnz") {
        if (Operand test, count; ls >> test >> count) {
          ins = JumpIfNotZero{test, count};
          return is;
        }
      }
      if (type == "inc" || type == "dec") {
        if (Operand dst; ls >> dst) {
          ins = Add{dst, Literal{type == "dec" ? -1 : 1}};
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

using Memory = std::array<int, 4>;

// https://www.reddit.com/r/cpp/comments/kst2pu/comment/giilcxv
// accessed 2023-11-30
template <std::size_t n>
constexpr decltype(auto) visit(auto&& func, auto&& var) {
#define aoc2016_variant_visit_case(c)       \
  case (c): {                               \
    if constexpr (n > (c)) {                \
      return func(*std::get_if<(c)>(&var)); \
    } else {                                \
      std::unreachable();                   \
    }                                       \
  } break
  switch (var.index()) {
    aoc2016_variant_visit_case(0);
    aoc2016_variant_visit_case(1);
    aoc2016_variant_visit_case(2);
  }
  std::unreachable();
#undef aoc2016_variant_visit_case
}

struct Read {
  Read() = delete;
  explicit Read(const Memory& m) : memory{m} {
  }

  int operator()(const Register& reg) const {
    return memory[reg.index];
  }
  int operator()(const Literal& lit) const {
    return lit.value;
  }

 private:
  const Memory& memory;
};

struct Write {
  Write() = delete;
  explicit Write(Memory& mem, int val) : memory{mem}, value{val} {
  }

  void operator()(const Register& reg) const {
    memory[reg.index] = value;
  }
  void operator()(const Literal&) const {
    throw std::invalid_argument{"can't write to a literal"};
  }

 private:
  Memory& memory;
  int value;
};

struct Execute {
  Execute() = delete;
  explicit Execute(Memory& mem) : memory{mem} {
  }

  std::ptrdiff_t operator()(const Add& add) const {
    const int lhs{visit<2>(Read{memory}, add.dst)};
    const int rhs{visit<2>(Read{memory}, add.src)};
    visit<2>(Write{memory, lhs + rhs}, add.dst);
    return 1;
  }
  std::ptrdiff_t operator()(const Copy& copy) const {
    const int value{visit<2>(Read{memory}, copy.src)};
    visit<2>(Write{memory, value}, copy.dst);
    return 1;
  }
  std::ptrdiff_t operator()(const JumpIfNotZero& jump) const {
    if (const int test{visit<2>(Read{memory}, jump.test)}; test != 0) {
      return visit<2>(Read{memory}, jump.count);
    }
    return 1;
  }

 private:
  Memory& memory;
};

void run(const auto& instructions, auto& memory) {
  for (std::ptrdiff_t pos{}; 0 <= pos && pos < instructions.size();) {
    pos += visit<3>(Execute{memory}, instructions[pos]);
  }
}

int main() {
  aoc::init_io();

  const auto instructions{views::istream<Instruction>(std::cin) | ranges::to<std::vector>()};

  std::array<int, 4> memory;

  run(instructions, memory);
  const auto part1{memory[0]};

  memory.fill(0);
  memory[2] = 1;
  run(instructions, memory);
  const auto part2{memory[0]};

  std::print("{} {}\n", part1, part2);

  return 0;
}
