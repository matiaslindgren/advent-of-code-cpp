#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Op {
  addi,
  addr,
  eqrr,
  gtrr,
  muli,
  mulr,
  seti,
  setr,
};

struct Instruction {
  Op op;
  int a{}, b{}, c{};
};

std::istream& operator>>(std::istream& is, Op& op) {
  if (std::string s; is >> s) {
    if (s == "addi"s) {
      op = Op::addi;
    } else if (s == "addr"s) {
      op = Op::addr;
    } else if (s == "eqrr"s) {
      op = Op::eqrr;
    } else if (s == "gtrr"s) {
      op = Op::gtrr;
    } else if (s == "muli"s) {
      op = Op::muli;
    } else if (s == "mulr"s) {
      op = Op::mulr;
    } else if (s == "seti"s) {
      op = Op::seti;
    } else if (s == "setr"s) {
      op = Op::setr;
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Op");
}

std::istream& operator>>(std::istream& is, Instruction& instruction) {
  if (Instruction ins; is >> ins.op >> ins.a >> ins.b >> ins.c) {
    instruction = ins;
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction");
}

auto parse_input(std::string path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int ip; is >> skip("#ip"s) >> ip) {
    const auto instructions{views::istream<Instruction>(is) | ranges::to<std::vector>()};
    if (is or is.eof()) {
      return std::pair{ip, instructions};
    }
  }
  throw std::runtime_error("failed parsing input");
}

using Memory = std::array<int, 6>;

void compute(Memory& mem, const Instruction& ins) {
  switch (ins.op) {
    case Op::addi: {
      mem[ins.c] = mem[ins.a] + ins.b;
    } break;
    case Op::addr: {
      mem[ins.c] = mem[ins.a] + mem[ins.b];
    } break;
    case Op::eqrr: {
      mem[ins.c] = mem[ins.a] == mem[ins.b];
    } break;
    case Op::gtrr: {
      mem[ins.c] = mem[ins.a] > mem[ins.b];
    } break;
    case Op::muli: {
      mem[ins.c] = mem[ins.a] * ins.b;
    } break;
    case Op::mulr: {
      mem[ins.c] = mem[ins.a] * mem[ins.b];
    } break;
    case Op::seti: {
      mem[ins.c] = ins.a;
    } break;
    case Op::setr: {
      mem[ins.c] = mem[ins.a];
    } break;
  }
}

auto run(Memory mem, const int ipr, const auto& instructions) {
  for (int& ip{mem[ipr]}; 0 <= ip and ip < instructions.size(); ip += 1) {
    if (ip == 2 and mem[4]) {
      // https://www.reddit.com/r/adventofcode/comments/a7j9zc/comment/ecgtijr
      // (2024-02-11)
      for (; mem[4] <= mem[2]; ++mem[4]) {
        if (mem[2] % mem[4] == 0) {
          mem[0] += mem[4];
        }
      }
      ip = 13;
    } else {
      compute(mem, instructions[ip]);
    }
  }
  return mem.front() + 1;
}

int main() {
  const auto [ip, instructions]{parse_input("/dev/stdin"s)};

  const auto part1{run({0, 0, 0, 0, 0, 0}, ip, instructions)};
  const auto part2{run({1, 0, 0, 0, 0, 0}, ip, instructions)};

  std::println("{} {}", part1, part2);

  return 0;
}
