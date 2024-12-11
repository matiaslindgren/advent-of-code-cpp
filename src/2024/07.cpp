#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Equation {
  long result;
  std::vector<int> inputs;
};

std::istream& operator>>(std::istream& is, Equation& equation) {
  using aoc::skip;
  using std::operator""s;

  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    if (long result; ls >> result) {
      if (ls >> skip(":"s)) {
        auto inputs{views::istream<int>(ls) | ranges::to<std::vector>()};
        if (ls.eof()) {
          equation = {result, inputs};
        }
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

long concat(long lhs, long rhs) {
  int width{1};
  while (rhs / width > 0) {
    width *= 10;
  }
  return width * lhs + rhs;
}

bool is_valid(const Equation& eq, bool use_concat) {
  for (std::vector stack{std::pair{0LL, eq.inputs.begin()}}; not stack.empty();) {
    auto [total, input]{stack.back()};
    stack.pop_back();

    if (total == eq.result and input == eq.inputs.end()) {
      return true;
    }
    if (total > eq.result or input == eq.inputs.end()) {
      continue;
    }

    stack.emplace_back(total + *input, input + 1);
    stack.emplace_back(total * *input, input + 1);
    if (use_concat) {
      stack.emplace_back(concat(total, *input), input + 1);
    }
  }
  return false;
}

auto search(const auto& equations) {
  long part1{};
  long part2{};
  for (const Equation& eq : equations) {
    if (is_valid(eq, false)) {
      part1 += eq.result;
    }
    if (is_valid(eq, true)) {
      part2 += eq.result;
    }
  }
  return std::pair{part1, part2};
}

int main() {
  const auto equations{aoc::parse_items<Equation>("/dev/stdin")};
  const auto [part1, part2]{search(equations)};
  std::println("{} {}", part1, part2);
  return 0;
}
