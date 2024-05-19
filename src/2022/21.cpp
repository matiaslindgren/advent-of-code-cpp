#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Expression {
  enum class BinOp : char {
    plus = '+',
    prod = '*',
    minus = '-',
    divide = '/',
  } op;
  int lhs, rhs;

  long eval(auto lhs, auto rhs) const {
    switch (op) {
      case BinOp::plus:
        return lhs + rhs;
      case BinOp::prod:
        return lhs * rhs;
      case BinOp::minus:
        return lhs - rhs;
      case BinOp::divide:
        return lhs / rhs;
    }
  }

  long eval_inverse(auto lhs, auto rhs) const {
    switch (op) {
      case BinOp::plus:
        return lhs - rhs;
      case BinOp::prod:
        return lhs / rhs;
      case BinOp::minus:
        return lhs + rhs;
      case BinOp::divide:
        return lhs * rhs;
    }
  }
};

struct Program {
  std::unordered_map<int, long> literals;
  std::unordered_map<int, Expression> expressions;

  static int job_hash(std::string_view job) {
    return ranges::fold_left(views::take(job, 4), 0, [n = 'z' - 'a' + 1](int id, char ch) {
      return id * n + (ch - 'a');
    });
  }

  std::optional<long> literal(const int id) const {
    if (literals.contains(id)) {
      return literals.at(id);
    }
    return std::nullopt;
  }

  void erase(const int id) {
    if (literals.contains(id)) {
      literals.erase(id);
    }
    if (expressions.contains(id)) {
      expressions.erase(id);
    }
  }

  void set_literal(const int id, long value) {
    erase(id);
    literals[id] = value;
  }

  void eval(const int id) {
    if (literal(id) or not expressions.contains(id)) {
      return;
    }
    Expression exp{expressions.at(id)};
    eval(exp.lhs);
    eval(exp.rhs);
    auto lhs{literal(exp.lhs)};
    auto rhs{literal(exp.rhs)};
    if (lhs and rhs) {
      set_literal(id, exp.eval(*lhs, *rhs));
    }
  }

  long solve(const int lhs_id, const int rhs_id, const int var_id) {
    auto lhs{literal(lhs_id)};
    auto rhs{literal(rhs_id)};
    if (lhs_id == var_id and rhs) {
      return *rhs;
    }
    if (rhs_id == var_id and lhs) {
      return *lhs;
    }

    double res;
    Expression exp;
    if (lhs) {
      res = lhs.value();
      exp = expressions.at(rhs_id);
    } else {
      res = rhs.value();
      exp = expressions.at(lhs_id);
    }

    int literal_id{};
    int expression_id{};
    {
      auto exp_lhs{literal(exp.lhs)};
      auto exp_rhs{literal(exp.rhs)};
      if (exp_lhs) {
        if (exp.op == Expression::BinOp::minus) {
          res = -res;
        }
        if (exp.op == Expression::BinOp::divide) {
          res = 1 / res;
        }
        literal_id = lhs_id;
        expression_id = exp.rhs;
        res = exp.eval_inverse(res, exp_lhs.value());
      } else {
        literal_id = rhs_id;
        expression_id = exp.lhs;
        res = exp.eval_inverse(res, exp_rhs.value());
      }
    }
    set_literal(literal_id, res);
    return solve(literal_id, expression_id, var_id);
  }
};

auto find_part1(Program prog) {
  auto root{Program::job_hash("root")};
  prog.eval(root);
  return prog.literals.at(root);
}

auto find_part2(Program prog) {
  auto root{Program::job_hash("root")};
  auto humn{Program::job_hash("humn")};
  prog.erase(humn);
  prog.eval(root);
  auto root_exp{prog.expressions.at(root)};
  return prog.solve(root_exp.lhs, root_exp.rhs, humn);
}

Program parse_program(std::string_view path) {
  Program prog;
  for (auto line : aoc::slurp_lines(path)) {
    ranges::replace(line, ':', ' ');
    bool ok{false};
    std::istringstream ls{line};
    if (std::string key; ls >> key and key.size() == 4) {
      int job_id{Program::job_hash(key)};
      if (std::string lhs; ls >> lhs and not lhs.empty()) {
        if (int literal;
            std::from_chars(lhs.c_str(), lhs.c_str() + lhs.size(), literal, 10).ec == std::errc{}) {
          prog.literals[job_id] = literal;
          ok = true;
        } else if (std::string op, rhs;
                   ls >> op and op.size() == 1 and ls >> rhs and rhs.size() == 4) {
          switch (op.front()) {
            case std::to_underlying(Expression::BinOp::plus):
            case std::to_underlying(Expression::BinOp::prod):
            case std::to_underlying(Expression::BinOp::minus):
            case std::to_underlying(Expression::BinOp::divide): {
              prog.expressions[job_id] = Expression{
                  .op = Expression::BinOp{op.front()},
                  .lhs = Program::job_hash(lhs),
                  .rhs = Program::job_hash(rhs),
              };
              ok = true;
            } break;
          }
        }
      }
    }
    if (not ok or not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line '{}'", line));
    }
  }
  return prog;
}

int main() {
  const auto prog{parse_program("/dev/stdin")};

  const auto part1{find_part1(prog)};
  const auto part2{find_part2(prog)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
