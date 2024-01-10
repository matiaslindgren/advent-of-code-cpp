module;
// MD5 algorithm references (accessed 2023-09-28)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

import std;

namespace ranges = std::ranges;
namespace views = std::views;

export module md5;

export namespace md5 {

using Byte = uint8_t;
using Message = std::vector<Byte>;
using Chunk = uint32_t;

Message parse_line(std::istream& is) {
  return views::istream<char>(is) | ranges::to<Message>();
}

inline void append_digits(Message& msg, const auto number) {
  std::vector<Byte> digits{};
  for (auto x{number}; x || digits.empty(); x /= 10) {
    digits.push_back('0' + (x % 10));
  }
  msg.append_range(digits | views::reverse);
}

struct num2hex {
  constexpr auto operator()(const auto x) const {
    return std::format("{:x}", x)[0];
  }
};

auto pack_input_chunks(ranges::sized_range auto&& msg) {
  std::array<Chunk, 16> chunks;
  auto it{msg.begin()};
  for (auto i{0u}; i < chunks.size(); ++i) {
    chunks[i] = 0;
    for (auto j{0u}; it != msg.end() && j < 4u; ++j) {
      chunks[i] |= *(it++) << (8 * j);
    }
  }
  return chunks;
}

inline Message compute(Message msg, int iterations = 1) {
  static std::array<Chunk, 64> md5_sine_table{};
  if (md5_sine_table.front() == 0) {
    for (auto&& [i, x] : views::zip(views::iota(1uz, md5_sine_table.size() + 1), md5_sine_table)) {
      x = static_cast<Chunk>((1LL << 32) * std::abs(std::sin(i)));
    }
  }
  static std::array<Chunk, 16> rotations
      = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};

  Message output;

  for (int iter{}; iter < iterations; ++iter) {
    Chunk chunks[4]{
        std::byteswap(0x01234567),
        std::byteswap(0x89abcdef),
        std::byteswap(0xfedcba98),
        std::byteswap(0x76543210),
    };

    {
      auto n_bits{8 * msg.size()};
      msg.push_back(0x80);
      msg.append_range(views::repeat(0, 64 - (msg.size() + 8) % 64));
      for (auto i{0}; i < 8; ++i) {
        msg.push_back(n_bits & 0xff);
        n_bits >>= 8;
      }
    }

    for (auto it{msg.begin()}; it != msg.end(); it += 64) {
      const auto input{pack_input_chunks(ranges::subrange(it, it + 64))};

      auto a{chunks[0]};
      auto b{chunks[1]};
      auto c{chunks[2]};
      auto d{chunks[3]};

      const auto update{[&](const auto i, auto f, const auto g) {
        f += a + md5_sine_table[i] + input[g];
        a = d;
        d = c;
        c = b;
        b += std::rotl(f, rotations[4 * (i / 16) + i % 4]);
      }};

      Chunk i{};
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

      chunks[0] += a;
      chunks[1] += b;
      chunks[2] += c;
      chunks[3] += d;
    }

    for (auto i{0uz}; i < 32; i += 8) {
      const auto chunk{std::byteswap(chunks[i / 8])};
      for (auto j{0uz}; j < 8uz; ++j) {
        output.push_back((chunk >> (28 - 4 * j)) & 0xf);
      }
    }

    if (iter + 1 < iterations) {
      msg.resize(output.size());
      ranges::transform(output, msg.begin(), num2hex{});
      output.clear();
    }
  }

  return output;
}

std::string hexdigest(const Message& msg) {
  return views::transform(msg, num2hex{}) | ranges::to<std::string>();
}

}  // namespace md5
