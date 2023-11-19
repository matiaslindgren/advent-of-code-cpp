import std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

std::size_t append_digits(md5::Message& msg, const auto msg_size, const auto number) {
  std::array<uint8_t, 16> digits = {0};
  std::size_t digit_count{0};
  for (auto x{number}; x; x /= 10) {
    digits[digit_count++] = '0' + (x % 10);
  }
  ranges::move(views::reverse(views::take(digits, digit_count)), msg.begin() + msg_size);
  return msg_size + digit_count;
}

std::pair<std::string, std::string> find_passwords(md5::Message msg, const std::size_t input_size) {
  std::string pw1;
  std::string pw2(8, 0);
  for (std::size_t i{}; i < 100'000'000 && (pw1.size() < 8 || pw2.find('\0') != std::string::npos);
       ++i) {
    const auto msg_len{append_digits(msg, input_size, i)};
    const auto checksum{md5::compute(msg, msg_len)};
    if (checksum & 0xfffff000) {
      continue;
    }
    const auto pw_idx{(checksum & 0xf00) >> 8};
    if (pw1.size() < 8) {
      pw1 += std::format("{:x}", pw_idx);
    }
    if (pw_idx < 8 && !pw2[pw_idx]) {
      const auto pw_val{(checksum & 0x0f0) >> 4};
      const auto pw_str{std::format("{:x}", pw_val)};
      pw2[pw_idx] = pw_str.front();
    }
  }
  return {pw1, pw2};
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

  const auto [part1, part2] = find_passwords(msg, input_size);
  std::print("{} {}\n", part1, part2);

  return 0;
}
