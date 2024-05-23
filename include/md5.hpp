#ifndef MD5_HEADER_INCLUDED
#define MD5_HEADER_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <format>
#include <ranges>

// MD5 algorithm references (accessed 2024-01-11)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

namespace md5 {

using Byte = uint8_t;
using Chunk = uint32_t;

using Input = std::array<Chunk, 16>;
using State = std::array<Chunk, 4>;

void compute_chunk(const Input& input, State& state) {
  static bool first_call{true};
  static std::array<Chunk, 64> T{};
  static std::array<Chunk, 64> rotations{};

  using std::views::iota;
  using std::views::zip;

  if (first_call) {
    first_call = false;

    for (auto&& [i, t] : zip(iota(1UZ), T)) {
      t = static_cast<Chunk>((1LL << 32) * std::abs(std::sin(i)));
    }

    constexpr std::array<Chunk, 16> rot{7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};
    for (auto&& [i, s] : zip(iota(0UZ), rotations)) {
      s = rot[4 * (i / 16) + i % 4];
    }
  }

  auto a{state[0]};
  auto b{state[1]};
  auto c{state[2]};
  auto d{state[3]};

  const auto update{[&](const auto i, auto f, const auto g) {
    f += a + T[i] + input[g];
    a = d;
    d = c;
    c = b;
    const auto s{rotations[i]};
    b += std::rotl(f, s);
  }};

  {
    unsigned i{};
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
  }

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
}

State sum(std::ranges::range auto&& msg) {
  State state{
      std::byteswap(0x01234567),
      std::byteswap(0x89abcdef),
      std::byteswap(0xfedcba98),
      std::byteswap(0x76543210),
  };

  Input input;
  input.fill(0);

  const auto append_input{[&input](auto i, Byte b) {
    const auto [i_inp, i_out]{std::div(i, 4)};
    input[i_inp] |= b << (8 * i_out);
  }};

  auto msg_size{0UZ};

  for (auto ch : msg) {
    append_input(msg_size % 64, ch);
    if (++msg_size % 64 == 0) {
      compute_chunk(input, state);
      input.fill(0);
    }
  }

  {
    append_input(msg_size % 64, 0x80);
    for (auto tail{msg_size + 1}; tail % 64 != 56; ++tail) {
      if (tail % 64 == 0) {
        compute_chunk(input, state);
        input.fill(0);
      }
      append_input(tail % 64, 0);
    }
    auto n_bits{8 * msg_size};
    for (auto i{56u}; i < 64; ++i) {
      append_input(i, n_bits & 0xff);
      n_bits >>= 8;
    }
    compute_chunk(input, state);
  }

  return state;
}

Chunk sum32bit(std::ranges::range auto&& msg) {
  return std::byteswap(sum(msg)[0]);
}

std::string hexdigest(const State& state) {
  return std::ranges::fold_left(state, std::string{}, [](auto&& res, auto&& chunk) {
    return std::format("{}{:08x}", res, std::byteswap(chunk));
  });
}

}  // namespace md5

#endif  // MD5_HEADER_INCLUDED
