#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr std::array delimiters{
    '(',
    ')',
    '[',
    ']',
    '{',
    '}',
    '<',
    '>',
};

char get_right(char l) {
  switch (l) {
    case '(':
    case '[':
    case '{':
    case '<':
      return ranges::find(delimiters, l)[1];
  }
  return 0;
}

int get_score(char c) {
  switch (c) {
    case '(':
      return 1;
    case ')':
      return 3;
    case '[':
      return 2;
    case ']':
      return 57;
    case '{':
      return 3;
    case '}':
      return 1197;
    case '<':
      return 4;
    case '>':
      return 25137;
  }
  return 0;
}

auto parse_line(const std::string& line) {
  std::string opened, invalid;
  for (char lhs : line) {
    if (char rhs{get_right(lhs)}) {
      opened.push_back(lhs);
    } else if (not opened.empty()) {
      rhs = opened.back();
      opened.pop_back();
      if (get_right(rhs) != lhs) {
        invalid.push_back(lhs);
      }
    } else {
      throw std::runtime_error("invalid state with unbalanced chunks");
    }
  }
  return std::pair{opened, invalid};
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto median(auto v) {
  ranges::nth_element(v, v.begin() + v.size() / 2);
  return v[v.size() / 2];
}

auto search(const auto& lines) {
  auto scores{
      views::transform(
          lines,
          [](auto line) {
            long score1{}, score2{};
            auto [opened, invalid]{parse_line(line)};
            if (not invalid.empty()) {
              score1 = get_score(invalid.front());
            } else {
              score2 = ranges::fold_left(opened | views::reverse, 0L, [](auto score, char ch) {
                return 5 * score + get_score(ch);
              });
            }
            return std::pair{score1, score2};
          }
      )
      | ranges::to<std::vector>()
  };
  return std::pair{
      sum(scores | views::elements<0>),
      median(
          scores | views::filter([](auto s) { return s.first == 0; })
          | views::elements<1> | ranges::to<std::vector>()
      ),
  };
}

int main() {
  const auto lines{aoc::parse_items<std::string>("/dev/stdin")};
  const auto [part1, part2]{search(lines)};
  std::println("{} {}", part1, part2);
  return 0;
}
