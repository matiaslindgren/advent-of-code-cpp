#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto infix_to_postfix(std::string_view expression, bool equal_precedence) {
  std::string ops;
  std::string out;
  for (auto [i, token] : my_std::views::enumerate(expression)) {
    switch (token) {
      case ' ': {
      } break;
      case ')': {
        if (auto i{ops.find_last_of('(')}; i != std::string::npos) {
          out.append_range(views::reverse(ops.substr(i + 1)));
          ops = ops.substr(0, i);
        } else {
          throw std::runtime_error(
              std::format("unbalanced closing paren in expression '{}'", expression)
          );
        }
      } break;
      case '*':
      case '+': {
        while (not ops.empty() and (ops.back() == '+' or (equal_precedence and ops.back() == '*'))
        ) {
          out.push_back(ops.back());
          ops.pop_back();
        }
        [[fallthrough]];
      }
      case '(': {
        ops.push_back(token);
      } break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        out.push_back(token);
      } break;
      default: {
        throw std::runtime_error(
            std::format("unexpected token '{}' in expression '{}'", token, expression)
        );
      }
    }
  }
  out.append_range(views::reverse(ops));
  return out;
}

auto compute(const auto& expressions, bool equal_precedence) {
  long s{};
  for (const auto& infix : expressions) {
    auto postfix{infix_to_postfix(infix, equal_precedence)};
    std::vector<long> res;
    for (char token : postfix) {
      switch (token) {
        case '*': {
          auto x{res.back()};
          res.pop_back();
          res.back() *= x;
        } break;
        case '+': {
          auto x{res.back()};
          res.pop_back();
          res.back() += x;
        } break;
        default: {
          res.push_back(token - '0');
        } break;
      }
    }
    s += res.front();
  }
  return s;
}

int main() {
  const auto expressions{aoc::slurp_lines("/dev/stdin")};

  const auto part1{compute(expressions, true)};
  const auto part2{compute(expressions, false)};

  std::println("{} {}", part1, part2);

  return 0;
}
