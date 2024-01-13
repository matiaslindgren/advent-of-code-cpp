import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

// TODO libc++18?
auto chunks2(auto&& r) {
  return views::zip(r, views::drop(r, 1)) | my_std::views::stride(2);
}

auto search_checksum(const std::string& input, const auto length) {
  std::vector<bool> bits(length + 1);
  for (const auto [i, b] : my_std::views::enumerate(input)) {
    bits[i] = b == '1';
  }

  auto n{input.size()};

  while (n < length) {
    bits[n] = 0;
    const auto n_take{std::min(n, length - n)};
    ranges::transform(
        bits | views::take(n) | views::reverse | views::take(n_take),
        ranges::next(bits.begin(), n + 1),
        std::logical_not{}
    );
    n += 1 + n_take;
  }

  for (n = length; n % 2 == 0; n /= 2) {
    ranges::transform(chunks2(bits | views::take(n)), bits.begin(), [](const auto& b1b2) {
      const auto [b1, b2] = b1b2;
      return b1 == b2;
    });
  }

  return views::transform(bits | views::take(n), [](const bool b) { return b ? '1' : '0'; })
         | ranges::to<std::string>();
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string input;
  std::cin >> input;

  const auto part1{search_checksum(input, 272)};
  const auto part2{search_checksum(input, 35651584)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
