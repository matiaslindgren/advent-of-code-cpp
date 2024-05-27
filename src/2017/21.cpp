#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

auto square_indices(const auto n) {
  auto&& side{views::iota(0, n)};
  return my_std::views::cartesian_product(side, side);
}

auto int_sqrt(const auto sq) {
  auto n{1};
  while (n * n < sq) {
    ++n;
  }
  return n;
}

struct Pattern {
  int size{};
  std::string str;

  [[nodiscard]]
  Pattern get_slice(const auto y0, const auto x0, const auto n) const {
    std::string res(n * n, 0);
    for (auto&& [y, x] : square_indices(n)) {
      res[y * n + x] = str[(y0 + y) * size + (x0 + x)];
    }
    return {n, res};
  }

  void set_slice(const auto y0, const auto x0, const Pattern& s) {
    for (auto&& [y, x] : square_indices(s.size)) {
      str[(y0 + y) * size + (x0 + x)] = s.str[y * s.size + x];
    }
  }

  [[nodiscard]]
  Pattern expand(const auto step, const auto& rules) const {
    const auto slice_size{step * step};
    const auto slice_count{str.size() / slice_size};
    Pattern res{
        .size = (step + 1) * int_sqrt(slice_count),
        .str = std::string(str.size() + slice_count * (2 * step + 1), 0),
    };
    for (auto&& [y, x] : square_indices(size / step)) {
      const auto y0{y * step};
      const auto x0{x * step};
      const auto y1{y * (step + 1)};
      const auto x1{x * (step + 1)};
      res.set_slice(y1, x1, rules.at(get_slice(y0, x0, step).str));
    }
    return res;
  }

  [[nodiscard]]
  Pattern transpose() const {
    std::string res(str.size(), 0);
    for (auto&& [y, x] : square_indices(size)) {
      res[x * size + y] = str[y * size + x];
    }
    return {size, res};
  }

  [[nodiscard]]
  Pattern flip() const {
    std::string res(str.size(), 0);
    for (auto&& [y, x] : square_indices(size)) {
      res[(size - (y + 1)) * size + x] = str[y * size + x];
    }
    return {size, res};
  }
};

struct Rule {
  Pattern src;
  Pattern dst;
};

auto expand(const auto& rules, const auto iterations) {
  std::vector<Pattern> expanded{Pattern{.size = 3, .str = ".#...####"s}};
  for (int iter{}; iter < iterations; ++iter) {
    const auto& pattern{expanded.back()};
    if (const auto n{pattern.str.size()}; n % 2 == 0) {
      expanded.push_back(pattern.expand(2, rules));
    } else if (n % 3 == 0) {
      expanded.push_back(pattern.expand(3, rules));
    } else {
      throw std::runtime_error(std::format("size == {} ???", n));
    }
  }
  return expanded;
}

std::istream& operator>>(std::istream& is, Pattern& pattern) {
  if (std::string s; is >> s) {
    if (const auto w{1 + ranges::count(s, '/')}; 2 <= w and w <= 4) {
      std::erase(s, '/');
      if (s.size() == w * w) {
        pattern = {.size = static_cast<int>(w), .str = s};
      }
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (Pattern src, dst; is >> src >> std::ws >> skip("=>"s) >> dst) {
    rule = {src, dst};
  }
  return is;
}

auto parse_rules(std::string_view path) {
  std::unordered_map<std::string, Pattern> rules;
  std::istringstream is{aoc::slurp_file(path)};
  for (const Rule& rule : views::istream<Rule>(is)) {
    Pattern src{rule.src};
    for (int i{}; i < 4; ++i) {
      src = src.transpose();
      rules[src.str] = rule.dst;
      src = src.flip();
      rules[src.str] = rule.dst;
    }
  }
  return rules;
}

int main() {
  const auto rules{parse_rules("/dev/stdin")};

  const auto expanded{expand(rules, 18)};
  const auto part1{ranges::count(expanded[5].str, '#')};
  const auto part2{ranges::count(expanded[18].str, '#')};

  std::println("{} {}", part1, part2);

  return 0;
}
