#include "aoc.hpp"
#include "std.hpp"

using std::operator""s;
using std::operator""sv;

namespace ranges = std::ranges;
namespace views = std::views;

using CharBits = std::bitset<'z' - 'a' + 1>;

auto search(std::string_view path) {
  int part1{}, part2{};
  for (auto&& section : views::split(aoc::slurp_file(path), "\n\n"s)) {
    CharBits any, all;
    all.set();
    for (auto&& line : views::split(section, "\n"sv)) {
      if (not line.empty()) {
        CharBits ans;
        for (char ch : line) {
          if ('a' <= ch and ch <= 'z') {
            ans.set('z' - ch);
          } else {
            throw std::runtime_error("all input chars must be in [a-z]");
          }
        }
        any |= ans;
        all &= ans;
      }
    }
    part1 += any.count();
    part2 += all.count();
  }
  return std::pair{part1, part2};
}

int main() {
  const auto [part1, part2]{search("/dev/stdin")};
  std::println("{} {}", part1, part2);
  return 0;
}
