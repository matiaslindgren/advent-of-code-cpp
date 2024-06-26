#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::is_digit;

struct Statement {
  std::string lhs0;
  std::string lhs1;
  std::string dst;
  enum : unsigned char {
    Unknown,
    Assign,
    Not,
    And,
    Or,
    LShift,
    RShift,
  } gate{};
};

bool is_literal(const std::string& s) {
  return not s.empty() and is_digit(s.front());
}

uint16_t parse_literal(const std::string& s) {
  return std::stoul(s);
}

uint16_t compute_signal(const std::string& wire, auto& circuit) {
  if (is_literal(wire)) {
    return parse_literal(wire);
  }
  const auto& stmt{circuit.at(wire)};
  uint16_t result{};
  switch (stmt.gate) {
    case Statement::Assign: {
      result = compute_signal(stmt.lhs0, circuit);
    } break;
    case Statement::Not: {
      result = ~compute_signal(stmt.lhs0, circuit);
    } break;
    case Statement::And: {
      result = compute_signal(stmt.lhs0, circuit) & compute_signal(stmt.lhs1, circuit);
    } break;
    case Statement::Or: {
      result = compute_signal(stmt.lhs0, circuit) | compute_signal(stmt.lhs1, circuit);
    } break;
    case Statement::LShift: {
      result = compute_signal(stmt.lhs0, circuit) << compute_signal(stmt.lhs1, circuit);
    } break;
    case Statement::RShift: {
      result = compute_signal(stmt.lhs0, circuit) >> compute_signal(stmt.lhs1, circuit);
    } break;
    case Statement::Unknown:
    default:
      throw std::runtime_error("broken circuit");
  }
  circuit[wire] = {std::format("{}", result), "", wire, Statement::Assign};
  return result;
}

std::istream& operator>>(std::istream& is, Statement& s) {
  std::string str;
  std::vector<std::string> lhs;
  while (is >> str and str != "->") {
    lhs.push_back(str);
  }
  if (not lhs.empty() and lhs.size() < 4 and str == "->" and is >> str) {
    auto gate{Statement::Unknown};
    switch (lhs.size()) {
      case 1: {
        s.lhs0 = lhs[0];
        gate = Statement::Assign;
      } break;
      case 2: {
        if (lhs[0] == "NOT") {
          s.lhs0 = lhs[1];
          gate = Statement::Not;
        }
      } break;
      case 3: {
        s.lhs0 = lhs[0];
        s.lhs1 = lhs[2];
        if (lhs[1] == "AND") {
          gate = Statement::And;
        } else if (lhs[1] == "OR") {
          gate = Statement::Or;
        } else if (lhs[1] == "LSHIFT") {
          gate = Statement::LShift;
        } else if (lhs[1] == "RSHIFT") {
          gate = Statement::RShift;
        }
      } break;
    }
    if (gate != Statement::Unknown) {
      s.dst = str;
      s.gate = gate;
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed to parse Statement");
}

int main() {
  const auto circuit{
      aoc::parse_items<Statement>("/dev/stdin")
      | views::transform([](const auto& stmt) { return std::tuple{stmt.dst, stmt}; })
      | ranges::to<std::unordered_map<std::string, Statement>>()
  };

  auto circuit_part1{circuit};
  const auto part1{compute_signal("a", circuit_part1)};

  auto circuit_part2{circuit};
  circuit_part2["b"] = {std::format("{}", part1), "", "b", Statement::Assign};
  const auto part2{compute_signal("a", circuit_part2)};

  std::println("{} {}", part1, part2);

  return 0;
}
