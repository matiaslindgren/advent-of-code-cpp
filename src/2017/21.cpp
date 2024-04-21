#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto square_indices(const auto n) {
  auto&& side{views::iota(0, n)};
  return my_std::views::cartesian_product(side, side);
}

struct Pattern {
  int size{};
  std::string str{};

  Pattern get_slice(const auto, const auto, const auto) const;
  void set_slice(const auto, const auto, const Pattern&);
  Pattern expand(const auto, const auto&) const;
  Pattern transpose() const;
  Pattern flip() const;
};

struct Rule {
  Pattern src, dst;
};

std::istream& operator>>(std::istream& is, Pattern& pattern) {
  if (std::string s; is >> s) {
    if (const auto w{1 + ranges::count(s, '/')}; 2 <= w and w <= 4) {
      std::erase(s, '/');
      if (s.size() == w * w) {
        pattern = {.size = static_cast<int>(w), .str = s};
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Pattern");
}

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (Pattern src, dst; is >> src >> std::ws >> skip("=>"s) >> dst) {
    rule = {src, dst};
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Rule");
}

Pattern Pattern::get_slice(const auto y0, const auto x0, const auto n) const {
  std::string res(n * n, 0);
  for (auto&& [y, x] : square_indices(n)) {
    res[y * n + x] = str[(y0 + y) * size + (x0 + x)];
  }
  return {n, res};
}

void Pattern::set_slice(const auto y0, const auto x0, const Pattern& s) {
  for (auto&& [y, x] : square_indices(s.size)) {
    str[(y0 + y) * size + (x0 + x)] = s.str[y * s.size + x];
  }
}

auto int_sqrt(const auto sq) {
  auto n{1};
  while (n * n < sq) {
    ++n;
  }
  return n;
}

Pattern Pattern::expand(const auto step, const auto& rules) const {
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

Pattern Pattern::transpose() const {
  std::string res(str.size(), 0);
  for (auto&& [y, x] : square_indices(size)) {
    res[x * size + y] = str[y * size + x];
  }
  return {size, res};
}

Pattern Pattern::flip() const {
  std::string res(str.size(), 0);
  for (auto&& [y, x] : square_indices(size)) {
    res[(size - (y + 1)) * size + x] = str[y * size + x];
  }
  return {size, res};
}

auto parse_rules(std::istream& is) {
  std::unordered_map<std::string, Pattern> rules;
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

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto rules{parse_rules(input)};

  const auto expanded{expand(rules, 18)};
  const auto part1{ranges::count(expanded[5].str, '#')};
  const auto part2{ranges::count(expanded[18].str, '#')};

  std::print("{} {}\n", part1, part2);

  return 0;
}
