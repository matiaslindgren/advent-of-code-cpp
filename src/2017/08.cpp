#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Instruction {
  std::function<int(int, int)> op;
  std::string key;
  int value;
};

struct Condition {
  std::function<bool(int, int)> comp;
  std::string key;
  int value;
};

struct Statement {
  Instruction ins;
  Condition cond;
};

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (std::string key, type; is >> key >> type and not key.empty()) {
    if (int value; is >> value) {
      decltype(Instruction::op) op{};
      if (type == "dec"s) {
        op = std::minus{};
      } else if (type == "inc"s) {
        op = std::plus{};
      } else {
        is.setstate(std::ios_base::failbit);
      }
      if (is) {
        ins = {op, key, value};
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction");
}

std::istream& operator>>(std::istream& is, Condition& cond) {
  if (std::string key; is >> std::ws >> skip("if"s) >> key and not key.empty()) {
    if (std::string type; is >> type) {
      if (int value; is >> value) {
        decltype(Condition::comp) comp{};
        if (type == "!="s) {
          comp = std::not_equal_to{};
        } else if (type == "<"s) {
          comp = std::less{};
        } else if (type == "<="s) {
          comp = std::less_equal{};
        } else if (type == "=="s) {
          comp = std::equal_to{};
        } else if (type == ">"s) {
          comp = std::greater{};
        } else if (type == ">="s) {
          comp = std::greater_equal{};
        } else {
          is.setstate(std::ios_base::failbit);
        }
        if (is) {
          cond = {comp, key, value};
        }
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Condition");
}

std::istream& operator>>(std::istream& is, Statement& stmt) {
  if (Instruction ins; is >> ins) {
    if (Condition cond; is >> cond) {
      stmt = {ins, cond};
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Statement");
}

int main() {
  const auto statements{aoc::parse_items<Statement>("/dev/stdin")};

  std::unordered_map<std::string, int> memory;
  int part2{};
  for (const auto& [ins, cond] : statements) {
    if (cond.comp(memory[cond.key], cond.value)) {
      part2 = std::max(part2, (memory[ins.key] = ins.op(memory[ins.key], ins.value)));
    }
  }

  const auto part1{ranges::max(views::values(memory))};

  std::println("{} {}", part1, part2);

  return 0;
}
