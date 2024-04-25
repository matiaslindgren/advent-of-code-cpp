#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Rule {
  std::vector<int> symbols;
  bool a{}, b{};

  void add(int s) {
    symbols.push_back(s);
  }
};

struct Statement {
  int key{};
  std::vector<Rule> rules{};

  void add(Rule r) {
    rules.push_back(r);
  }
};

std::istream& operator>>(std::istream& is, Statement& statement) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    Statement stmt;
    std::istringstream ls{line + " | "s};
    if (int key; ls >> key >> skip(":"s)) {
      stmt.key = key;
      if (line.contains("\"a\""s) and ls >> std::ws >> skip("\"a\" |"s)) {
        stmt.add(Rule{.a = true});
      } else if (line.contains("\"b\""s) and ls >> std::ws >> skip("\"b\" |"s)) {
        stmt.add(Rule{.b = true});
      } else {
        for (Rule rule; ls;) {
          if (ls >> std::ws and ls.peek() == '|' and ls.get()) {
            stmt.add(rule);
            rule = {};
          } else if (int symbol; ls >> symbol) {
            rule.add(symbol);
          }
        }
      }
    }
    if (not(ls >> std::ws).eof()) {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
    statement = stmt;
  }
  return is;
}

std::string build_regex(const auto& statements, const std::size_t lineno, const bool part2) {
  // adapted from
  // https://github.com/sophiebits/adventofcode/blob/a5cb8d852884608f5fe7a622c2cc50fe0f6f9496/2020/day19.py
  // accessed 2024-04-19
  if (part2 and lineno == 8) {
    return build_regex(statements, 42, part2) + "+"s;
  }
  std::string re{"(?:"s};
  if (part2 and lineno == 11) {
    const auto a{build_regex(statements, 42, part2)};
    const auto b{build_regex(statements, 31, part2)};
    for (int n{1}; n < 10; ++n) {
      if (n > 1) {
        re += "|"s;
      }
      re += std::format("{}{{{}}}{}{{{}}}", a, n, b, n);
    }
  }
  const Statement& stmt{statements.at(lineno)};
  for (auto [i, rule] : my_std::views::enumerate(stmt.rules)) {
    if (rule.a) {
      return "a"s;
    }
    if (rule.b) {
      return "b"s;
    }
    if (i > 0) {
      re += "|"s;
    }
    for (int s : rule.symbols) {
      re += build_regex(statements, s, part2);
    }
  }
  return re + ")"s;
}

auto search(auto statements, const auto& input) {
  ranges::sort(statements, {}, &Statement::key);
  std::regex re1{build_regex(statements, 0, false)};
  std::regex re2{build_regex(statements, 0, true)};
  int part1{}, part2{};
  for (const auto& inp : input) {
    part1 += std::regex_match(inp, re1);
    part2 += std::regex_match(inp, re2);
  }
  return std::pair{part1, part2};
}

auto parse_input(std::string path) {
  const auto input{aoc::slurp_file(path)};
  const auto sections{
      views::split(input, "\n\n"s)
      | views::transform([](auto&& r) { return ranges::to<std::string>(r); })
      | ranges::to<std::vector<std::string>>()
  };
  if (sections.size() != 2) {
    throw std::runtime_error("input must contain 2 sections separated by 2 newlines");
  }

  std::istringstream is{sections[0]};
  auto statements{views::istream<Statement>(is) | ranges::to<std::vector>()};
  if (is.eof()) {
    is = std::istringstream(sections[1]);
    auto lines{views::istream<std::string>(is) | ranges::to<std::vector>()};
    if (is.eof()) {
      return std::pair{statements, lines};
    }
  }
  throw std::runtime_error("failed parsing input sections");
}

int main() {
  const auto [statements, input]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(statements, input)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
