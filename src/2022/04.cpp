#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

using Pair = std::pair<int, int>;

auto find_part1(const auto& pairs) {
  return ranges::count_if(pairs, [](auto p) {
    auto [lhs, rhs]{p};
    auto [l1, l2]{lhs};
    auto [r1, r2]{rhs};
    return (l1 <= r1 and r2 <= l2) or (r1 <= l1 and l2 <= r2);
  });
}

auto find_part2(const auto& pairs) {
  return ranges::count_if(pairs, [](auto p) {
    auto [lhs, rhs]{p};
    auto [l1, l2]{lhs};
    auto [r1, r2]{rhs};
    return (l1 <= r1 and r1 <= l2) or (r1 <= l1 and l1 <= r2);
  });
}

auto parse_pairs(std::string_view path) {
  std::vector<std::pair<Pair, Pair>> pairs;
  {
    const auto lines{aoc::slurp_lines(path)};
    for (auto line : lines) {
      std::istringstream ls{line};
      if (int l1{}, l2{}, r1{}, r2{};
          ls >> l1 >> skip("-"s) >> l2 >> skip(","s) >> r1 >> skip("-"s) >> r2) {
        pairs.emplace_back(Pair{l1, l2}, Pair{r1, r2});
      }
      if (not ls.eof()) {
        throw std::runtime_error(std::format("failed parsing line '{}'", line));
      }
    }
  }
  return pairs;
}

int main() {
  const auto pairs{parse_pairs("/dev/stdin")};

  const auto part1{find_part1(pairs)};
  const auto part2{find_part2(pairs)};

  std::println("{} {}", part1, part2);

  return 0;
}
