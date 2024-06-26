#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Op : signed char {
  addi = 0,
  addr,
  bani,
  banr,
  bori,
  borr,
  eqir,
  eqri,
  eqrr,
  gtir,
  gtri,
  gtrr,
  muli,
  mulr,
  seti,
  setr,
};
const std::unordered_map<std::string, Op> str2op{
    {"addi"s, Op::addi},
    {"addr"s, Op::addr},
    {"bani"s, Op::bani},
    {"banr"s, Op::banr},
    {"bori"s, Op::bori},
    {"borr"s, Op::borr},
    {"eqir"s, Op::eqir},
    {"eqri"s, Op::eqri},
    {"eqrr"s, Op::eqrr},
    {"gtir"s, Op::gtir},
    {"gtri"s, Op::gtri},
    {"gtrr"s, Op::gtrr},
    {"muli"s, Op::muli},
    {"mulr"s, Op::mulr},
    {"seti"s, Op::seti},
    {"setr"s, Op::setr},
};

struct Instruction {
  Op op{};
  long a{};
  long b{};
  long c{};
};

auto run(const auto& instructions) {
  // https://www.reddit.com/r/adventofcode/comments/a86jgt/comment/ec8lyck
  // (2024-02-17)
  long part1{-1};
  long part2{-1};
  long c{};
  for (std::unordered_set<long> seen;;) {
    auto a{c | 65536};
    c = instructions[7].a;
    for (; part2 != c;) {
      c = (((c + (a & 255)) & 16777215) * 65899) & 16777215;
      if (a < 256) {
        if (auto&& [_, unseen]{seen.insert(c)}; not unseen) {
          return std::pair{part1, part2};
        }
        part1 = part1 < 0 ? c : part1;
        part2 = c;
      } else {
        a /= 256;
      }
    }
  }
}

std::istream& operator>>(std::istream& is, Op& op) {
  if (std::string s; is >> s) {
    if (str2op.contains(s)) {
      op = str2op.at(s);
    } else {
      throw std::runtime_error(std::format("unknown elf code opcode '{}'", s));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& instruction) {
  if (Instruction ins; is >> ins.op >> ins.a >> ins.b >> ins.c) {
    instruction = ins;
  }
  return is;
}

auto parse_input(std::string path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int ip{}; is >> skip("#ip"s) >> ip) {
    const auto instructions{views::istream<Instruction>(is) | ranges::to<std::vector>()};
    if (is or is.eof()) {
      return std::pair{ip, instructions};
    }
  }
  throw std::runtime_error("input is not elf code (aoc year 2018)");
}

int main() {
  const auto [_, instructions]{parse_input("/dev/stdin")};
  const auto [part1, part2]{run(instructions)};
  std::println("{} {}", part1, part2);
  return 0;
}
