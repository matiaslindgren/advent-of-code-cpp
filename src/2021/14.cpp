#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

// TODO (llvm19) ranges::pairwise
auto pairwise(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

auto make_counter(auto&& v) {
  return v | views::transform([](auto&& key) { return std::pair{key, 0LL}; })
         | ranges::to<std::unordered_map>();
}

auto apply_steps(std::string polymer, auto rules, int n_steps) {
  auto pair_counts{make_counter(rules | views::keys)};
  for (auto [ch0, ch1] : pairwise(polymer)) {
    pair_counts[{ch0, ch1}] += 1;
  }

  auto char_counts{make_counter(rules | views::values)};
  for (char ch : polymer) {
    char_counts[ch] += 1;
  }

  for (int s{}; s < n_steps; ++s) {
    for (auto [pair, count] : pair_counts | ranges::to<std::vector>()) {
      if (count > 0) {
        char ch0{pair.at(0)};
        char ch1{rules.at(pair)};
        char ch2{pair.at(1)};
        char_counts[ch1] += count;
        pair_counts[{ch0, ch2}] -= count;
        pair_counts[{ch0, ch1}] += count;
        pair_counts[{ch1, ch2}] += count;
      }
    }
  }

  auto [min, max]{ranges::minmax(char_counts | views::values)};
  return max - min;
}

bool is_valid(const std::string& s) {
  return not s.empty() and ranges::all_of(s, [](char ch) { return 'A' <= ch and ch <= 'Z'; });
}

auto parse_input(std::string path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (std::string polymer; is >> polymer and is_valid(polymer)) {
    std::unordered_map<std::string, char> rules;
    for (std::string src, dst; is >> src and is_valid(src) and src.size() == 2
                               and is >> std::ws >> skip("->"s) >> dst and is_valid(dst)
                               and dst.size() == 1;) {
      rules[src] = dst[0];
    }
    if (not rules.empty() and is.eof()) {
      return std::pair{polymer, rules};
    }
    throw std::runtime_error("failed parsing rules");
  }
  throw std::runtime_error("failed parsing polymer");
}

int main() {
  const auto [polymer, rules]{parse_input("/dev/stdin")};

  const auto part1{apply_steps(polymer, rules, 10)};
  const auto part2{apply_steps(polymer, rules, 40)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
