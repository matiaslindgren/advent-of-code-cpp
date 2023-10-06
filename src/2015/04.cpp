import std;

namespace ranges = std::ranges;
namespace views = std::views;

// MD5 algorithm references (accessed 2023-09-28)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

auto pack_md5_input(std::array<uint8_t, 64>& msg, const auto msg_len) {
  std::array<uint32_t, 16> input = {0};
  {
    msg[msg_len] = 0x80;
    const auto bit_count = 8 * msg_len;
    for (auto i : views::iota(msg_len + 1, msg.size())) {
      msg[i] = (i < 56) ? 0 : (bit_count >> (8 * (i - 56))) & 0xff;
    }
  }
  for (auto i : views::iota(0u, input.size())) {
    for (auto j : views::iota(0u, 4u)) {
      input[i] |= msg[4 * i + j] << (8 * j);
    }
  }
  return input;
}

uint32_t partial_md5(std::array<uint8_t, 64>& msg, const auto msg_len) {
  static constexpr std::array<uint32_t, 16> md5_rotations
      = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};

  static std::array<uint32_t, 64> md5_sine_table = {0};
  if (md5_sine_table.front() == 0) {
    for (auto&& [i, x] : views::zip(views::iota(1uz, md5_sine_table.size() + 1),
                                    md5_sine_table)) {
      x = static_cast<uint32_t>((1LL << 32) * std::abs(std::sin(i)));
    }
  }

  uint32_t a0{std::byteswap(0x01234567)};
  uint32_t b0{std::byteswap(0x89abcdef)};
  uint32_t c0{std::byteswap(0xfedcba98)};
  uint32_t d0{std::byteswap(0x76543210)};

  const std::array<uint32_t, 16> input = pack_md5_input(msg, msg_len);
  auto a{a0};
  auto b{b0};
  auto c{c0};
  auto d{d0};

  for (auto i : views::iota(uint32_t{0}, uint32_t{64})) {
    uint32_t f{0};
    uint32_t g{0};
    if (i < 16) {
      f = (b & c) | (~b & d);
      g = i;
    } else if (i < 32) {
      f = (b & d) | (~d & c);
      g = (5 * i + 1) % 16;
    } else if (i < 48) {
      f = b ^ c ^ d;
      g = (3 * i + 5) % 16;
    } else {
      f = c ^ (b | ~d);
      g = (7 * i) % 16;
    }
    f += a + md5_sine_table[i] + input[g];
    a = d;
    d = c;
    c = b;
    const auto s = md5_rotations[4 * (i / 16) + i % 4];
    b += std::rotl(f, s);
  }

  return std::byteswap(a + a0);
}

std::size_t append_digits(std::array<uint8_t, 64>& msg,
                          const auto msg_size,
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

  std::array<uint8_t, 64> msg = {0};
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
      const auto res = partial_md5(msg, msg_len);
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
