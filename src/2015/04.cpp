import std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

std::size_t append_digits(md5::Message& msg, const auto msg_size,
                          const auto number) {
  std::array<uint8_t, 16> digits = {0};
  std::size_t digit_count{0};
  for (auto x{number}; x; x /= 10) {
    digits[digit_count++] = '0' + (x % 10);
  }
  ranges::move(views::reverse(views::take(digits, digit_count)),
               ranges::next(msg.begin(), msg_size));
  return msg_size + digit_count;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  md5::Message msg = {0};
  std::size_t input_size{};
  {
    std::string input;
    std::cin >> input;
    input_size = input.size();
    ranges::move(input, msg.begin());
  }

  const auto find_next{[&](const auto begin, const auto num_zeros) {
    auto i{begin};
    for (; i < 10'000'000; ++i) {
      const auto msg_len = append_digits(msg, input_size, i);
      const auto res = md5::compute(msg, msg_len);
      if ((res >> (32 - 4 * num_zeros)) == 0) {
        break;
      }
    }
    return i;
  }};

  const auto part1{find_next(0uz, 5)};
  const auto part2{find_next(part1, 6)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
