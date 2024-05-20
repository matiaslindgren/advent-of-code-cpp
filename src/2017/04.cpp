#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::vector<std::string>;

bool is_valid(const Strings& passphrases) {
  std::unordered_map<std::string, int> freq;
  for (const auto& pp : passphrases) {
    freq[pp] += 1;
  }
  return ranges::all_of(freq | views::values, [](int f) { return f == 1; });
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  int part1{}, part2{};

  for (std::string line; std::getline(input, line);) {
    std::istringstream ls{line};
    Strings passphrases{views::istream<std::string>(ls) | ranges::to<Strings>()};

    part1 += is_valid(passphrases);

    ranges::for_each(passphrases, [](auto& pp) { ranges::sort(pp); });
    part2 += is_valid(passphrases);
  }

  std::println("{} {}", part1, part2);

  return 0;
}
