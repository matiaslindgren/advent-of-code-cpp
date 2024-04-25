#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using HashState = std::array<uint8_t, 256>;

HashState compute_hash(const auto& input, int rounds) {
  HashState state;
  // TODO (llvm19?) ranges::iota(state, 0u);
  for (unsigned x{}; x < state.size(); ++x) {
    state[x] = x;
  }
  std::size_t n{state.size()}, pos{}, skip{};
  for (int r{}; r < rounds; ++r) {
    for (auto len : input) {
      auto lhs{pos};
      auto rhs{(pos + len + n - 1) % n};
      for (int i{}; i < (len + 1) / 2; ++i) {
        std::swap(state[lhs], state[rhs]);
        lhs = (lhs + 1) % n;
        rhs = (rhs + n - 1) % n;
      }
      pos = (pos + len + skip) % n;
      skip += 1;
    }
  }
  return state;
}

int compute_part1(std::string input) {
  ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  const auto lengths{views::istream<int>(is) | ranges::to<std::vector>()};
  const auto state{compute_hash(lengths, 1)};
  return state[0] * state[1];
}

auto compute_part2(std::string input) {
  input.append_range(std::vector<char>{17, 31, 73, 47, 23});
  const auto state{compute_hash(input, 64)};
  // TODO join_with view
  std::string res;
  for (auto begin{0uz}; begin < state.size(); begin += 16) {
    const auto chunk{ranges::fold_left(
        state | views::drop(begin + 1) | views::take(15),
        state[begin],
        std::bit_xor{}
    )};
    res += std::format("{:02x}", chunk);
  }
  return res;
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string input;
  std::cin >> input;

  const auto part1{compute_part1(input)};
  const auto part2{compute_part2(input)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
