#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

using Pair = std::pair<int, int>;
using Constraint = std::pair<Pair, Pair>;

std::string join_digits(const auto& digits) {
  return digits | views::transform([](int d) { return std::format("{}", d); }) | views::join
         | ranges::to<std::string>();
}

auto search(const std::vector<Constraint>& constraints) {
  std::vector max(14, std::numeric_limits<int>::min());
  std::vector min(14, std::numeric_limits<int>::max());
  for (auto [lhs, rhs] : constraints) {
    auto [i1, x1]{rhs};
    auto [i2, x2]{lhs};
    for (int y2{1}; y2 < 10; ++y2) {
      for (int y1{1}; y1 < 10; ++y1) {
        if (y2 + x2 == y1 - x1) {
          max.at(i2) = std::max(max.at(i2), y2);
          max.at(i1) = std::max(max.at(i1), y1);
          min.at(i2) = std::min(min.at(i2), y2);
          min.at(i1) = std::min(min.at(i1), y1);
        }
      }
    }
  }
  return std::pair{join_digits(max), join_digits(min)};
}

auto parse_constraints(std::string path) {
  const auto input{aoc::slurp_file(path)};
  const auto sections{
      views::split(input, "inp w\n"s)
      | views::transform([](auto&& r) { return ranges::to<std::string>(r); })
      | views::filter([](std::string s) { return not s.empty(); })
      | ranges::to<std::vector<std::string>>()
  };
  if (sections.empty()) {
    throw std::runtime_error("input must contain at least one section separated by 'inp w'");
  }

  std::vector<Constraint> constraints;
  std::vector<Pair> stack;

  for (auto [i1, section] : my_std::views::enumerate(sections)) {
    std::vector<std::pair<std::string, std::string>> lines;

    for (std::istringstream is{section}; is;) {
      if (std::string a, b, c; is >> a >> b >> c >> std::ws) {
        if (a.size() == 3 and b.size() == 1 and c.size() > 0) {
          lines.emplace_back(a, c);
        } else {
          throw std::runtime_error("every line must contain 3 character binary operations");
        }
      }
    }

    int x1{std::atoi(lines.at(4).second.c_str())};
    int x2{std::atoi(lines.at(14).second.c_str())};
    if (lines.at(3).second == "26"s) {
      if (stack.empty()) {
        throw std::runtime_error("first subprogram must contain 'div z 1'");
      }
      auto p{stack.back()};
      stack.pop_back();
      const int i2{p.first};
      x2 = p.second;
      constraints.emplace_back(Pair{i2, x2}, Pair{i1, x1});
    } else {
      stack.emplace_back(i1, x2);
    }
  }

  return constraints;
}

int main() {
  const auto [part1, part2]{search(parse_constraints("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
