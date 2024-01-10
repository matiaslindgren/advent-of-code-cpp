module;
// MD5 algorithm references (accessed 2023-09-28)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

import std;

namespace ranges = std::ranges;
namespace views = std::views;

export module md5;

export namespace md5 {

struct Message {
  std::array<uint8_t, 64> data;
  std::size_t len;

  auto begin() {
    return ranges::begin(data);
  }
  auto end() {
    return ranges::next(ranges::begin(data), len, ranges::end(data));
  }
  // TODO deducing this
  auto& operator[](auto&& i) {
    return data[i];
  }
  const auto& operator[](auto&& i) const {
    return data[i];
  }
};

std::istream& operator>>(std::istream& is, Message& msg) {
  if (std::string s; is >> s) {
    ranges::copy(s, msg.begin());
    msg.len = s.size();
  }
  return is;
}

using Input = std::array<uint32_t, 16>;
using Digits = std::array<uint8_t, 32>;

inline void append_digits(Message& msg, const auto number) {
  std::array<uint8_t, 16> digits = {};
  std::size_t digit_count{};
  for (auto x{number}; x || !digit_count; x /= 10) {
    digits[digit_count++] = '0' + (x % 10);
  }
  ranges::move(digits | views::take(digit_count) | views::reverse, msg.end());
  msg.len += digit_count;
}

inline Input pack_md5_input(Message& msg) {
  {
    auto i{msg.len};
    msg[i] = 0x80;
    for (++i; i < 56; ++i) {
      msg[i] = 0;
    }
    const auto bit_count{8 * msg.len};
    for (; i < msg.data.size(); ++i) {
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

struct digit2hex {
  constexpr auto operator()(const auto digit) const {
    return std::format("{:x}", digit)[0];
  }
};

inline Digits compute(Message msg, long iterations = 1) {
  static std::array<uint32_t, 64> md5_sine_table{};
  if (md5_sine_table.front() == 0) {
    for (auto&& [i, x] : views::zip(views::iota(1uz, md5_sine_table.size() + 1), md5_sine_table)) {
      x = static_cast<uint32_t>((1LL << 32) * std::abs(std::sin(i)));
    }
  }
  static std::array<uint32_t, 16> rotations
      = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};

  constexpr uint32_t init[4]{
      std::byteswap(0x01234567),
      std::byteswap(0x89abcdef),
      std::byteswap(0xfedcba98),
      std::byteswap(0x76543210)
  };

  Digits output;

  for (long n_it{}; n_it < iterations; ++n_it) {
    const auto input{pack_md5_input(msg)};

    auto a{init[0]};
    auto b{init[1]};
    auto c{init[2]};
    auto d{init[3]};

    const auto update{[&](const auto i, auto f, const auto g) {
      f += a + md5_sine_table[i] + input[g];
      a = d;
      d = c;
      c = b;
      b += std::rotl(f, rotations[4 * (i / 16) + i % 4]);
    }};

    uint32_t i{};
    for (; i < 16u; ++i) {
      update(i, (b & c) | (~b & d), i);
    }
    for (; i < 32u; ++i) {
      update(i, (b & d) | (~d & c), (5 * i + 1) % 16);
    }
    for (; i < 48u; ++i) {
      update(i, b ^ c ^ d, (3 * i + 5) % 16);
    }
    for (; i < 64u; ++i) {
      update(i, c ^ (b | ~d), (7 * i) % 16);
    }

    const uint32_t chunks[4]{
        a + init[0],
        b + init[1],
        c + init[2],
        d + init[3],
    };

    for (auto i{0uz}; i < output.size(); i += 8) {
      const auto chunk{std::byteswap(chunks[i / 8])};
      for (auto j{0uz}; j < 8uz; ++j) {
        output[i + j] = (chunk >> (28 - 4 * j)) & 0xf;
      }
    }

    if (n_it + 1 < iterations) {
      ranges::transform(output, msg.begin(), digit2hex{});
      msg.len = output.size();
    }
  }

  return output;
}

}  // namespace md5
