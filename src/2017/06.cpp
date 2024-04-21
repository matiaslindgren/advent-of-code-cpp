#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Bank {
  std::vector<int> v;
  bool operator==(const Bank& bank) const {
    return ranges::equal(v, bank.v);
  }
};

template <>
struct std::hash<Bank> {
  std::size_t operator()(const Bank& bank) const noexcept {
    return my_std::ranges::fold_left(bank.v, 0uz, [](auto h, auto x) { return (h << 4) | x; });
  }
};

auto find_loop(Bank bank) {
  int cycle{};
  std::unordered_map<Bank, int> seen;
  for (; not seen.contains(bank); ++cycle) {
    seen[bank] = cycle;

    const auto begin{ranges::max_element(bank.v)};
    auto blocks{std::exchange(*begin, 0u)};

    const auto n{bank.v.size()};
    for (auto i{((begin - bank.v.begin()) + 1) % n}; blocks; i = (i + 1) % n) {
      bank.v[i] += 1;
      blocks -= 1;
    }
  }
  return std::pair{cycle, cycle - seen[bank]};
}

int main() {
  const auto bank{aoc::slurp<int>("/dev/stdin")};
  if (bank.size() > 16 or not ranges::all_of(bank, [](auto x) { return 0 <= x and x <= 15; })) {
    throw std::runtime_error(
        "invalid input, input size must be at most 16 and every element must fit into 4 bits"
    );
  }

  const auto [part1, part2] = find_loop(Bank{bank});
  std::print("{} {}\n", part1, part2);

  return 0;
}
