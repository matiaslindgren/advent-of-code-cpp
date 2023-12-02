module;
// MD5 algorithm references (accessed 2023-09-28)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

import std;

namespace ranges = std::ranges;
namespace views = std::views;

export module md5;

export namespace md5 {

using Message = std::array<uint8_t, 64>;
using Input = std::array<uint32_t, 16>;

inline std::size_t append_digits(Message& msg, const auto msg_size, const auto number) {
  std::array<uint8_t, 16> digits = {0};
  std::size_t digit_count{0};
  for (auto x{number}; x; x /= 10) {
    digits[digit_count++] = '0' + (x % 10);
  }
  ranges::move(digits | views::take(digit_count) | views::reverse, msg.begin() + msg_size);
  return msg_size + digit_count;
}

inline Input pack_md5_input(Message& msg, const auto msg_len) {
  {
    auto i{msg_len};
    msg[i] = 0x80;
    for (++i; i < 56; ++i) {
      msg[i] = 0;
    }
    const auto bit_count{8 * msg_len};
    for (; i < msg.size(); ++i) {
      msg[i] = (bit_count >> (8 * (i - 56))) & 0xff;
    }
  }
  Input input;
  for (auto i{0u}; i < input.size(); ++i) {
    const auto& chunk{ranges::next(msg.begin(), 4 * i)};
    input[i] = 0;
    for (auto j{0u}; j < 4u; ++j) {
      input[i] |= chunk[j] << (8 * j);
    }
  }
  return input;
}

inline uint32_t compute(Message msg, const auto msg_len) {
  static std::array<uint32_t, 64> md5_sine_table{};
  if (md5_sine_table.front() == 0) {
    for (auto&& [i, x] : views::zip(views::iota(1uz, md5_sine_table.size() + 1), md5_sine_table)) {
      x = static_cast<uint32_t>((1LL << 32) * std::abs(std::sin(i)));
    }
  }

  static constexpr uint32_t a0{std::byteswap(0x01234567)};
  static constexpr uint32_t b0{std::byteswap(0x89abcdef)};
  static constexpr uint32_t c0{std::byteswap(0xfedcba98)};
  static constexpr uint32_t d0{std::byteswap(0x76543210)};

  const auto input{pack_md5_input(msg, msg_len)};

  {
    auto a{a0};
    auto b{b0};
    auto c{c0};
    auto d{d0};

    const auto update{[&](const auto i, auto f, const auto g, const auto rot) {
      f += a + md5_sine_table[i] + input[g];
      a = d;
      d = c;
      c = b;
      b += std::rotl(f, rot);
    }};

    uint32_t i{};
    for (; i < 16u; ++i) {
      constexpr uint32_t rot[4] = {7, 12, 17, 22};
      update(i, (b & c) | (~b & d), i, rot[i % 4]);
    }
    for (; i < 32u; ++i) {
      constexpr uint32_t rot[4] = {5, 9, 14, 20};
      update(i, (b & d) | (~d & c), (5 * i + 1) % 16, rot[i % 4]);
    }
    for (; i < 48u; ++i) {
      constexpr uint32_t rot[4] = {4, 11, 16, 23};
      update(i, b ^ c ^ d, (3 * i + 5) % 16, rot[i % 4]);
    }
    for (; i < 64u; ++i) {
      constexpr uint32_t rot[4] = {6, 10, 15, 21};
      update(i, c ^ (b | ~d), (7 * i) % 16, rot[i % 4]);
    }

    return std::byteswap(a + a0);
  }
}

}  // namespace md5
