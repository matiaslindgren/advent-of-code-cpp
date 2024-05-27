#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

// TODO libc++19?
auto chunks2(auto&& r) {
  return views::zip(r, views::drop(r, 1)) | my_std::views::stride(2);
}

auto search_checksum(const std::string& input, const auto length) {
  std::vector<bool> bits(length + 1);
  for (auto&& [i, b] : my_std::views::enumerate(input)) {
    bits[i] = (b == '1');
  }

  auto n{input.size()};

  while (n < length) {
    bits[n] = false;
    const auto n_take{std::min(n, length - n)};
    ranges::transform(
        bits | views::take(n) | views::reverse | views::take(n_take),
        &bits.at(n + 1),
        std::logical_not{}
    );
    n += 1 + n_take;
  }

  for (n = length; n % 2 == 0; n /= 2) {
    ranges::transform(
        chunks2(bits | views::take(n)),
        bits.begin(),
        my_std::apply_fn(std::equal_to{})
    );
  }

  return views::transform(bits | views::take(n), [](bool b) { return b ? '1' : '0'; })
         | ranges::to<std::string>();
}

int main() {
  std::ios::sync_with_stdio(false);
  if (std::string input;
      std::cin >> input and ranges::all_of(input, [](char ch) { return ch == '1' or ch == '0'; })) {
    const auto part1{search_checksum(input, 272)};
    const auto part2{search_checksum(input, 35651584)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input bits");
}
