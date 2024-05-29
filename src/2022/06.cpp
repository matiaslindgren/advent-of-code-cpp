#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

auto find_distinct(std::string input, std::size_t msg_len) {
  auto rhs{msg_len};
  for (; rhs < input.size(); ++rhs) {
    auto window{input.substr(rhs - msg_len, msg_len)};
    if (auto distinct{ranges::to<std::unordered_set>(window)}; distinct.size() == msg_len) {
      break;
    }
  }
  return rhs;
}

int main() {
  const auto input{aoc::slurp_file("/dev/stdin")};

  const auto part1{find_distinct(input, 4)};
  const auto part2{find_distinct(input, 14)};

  std::println("{} {}", part1, part2);

  return 0;
}
