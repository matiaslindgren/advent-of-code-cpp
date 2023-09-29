import std;

// References (accessed 2023-09-28)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

auto pack_md5_input(const std::string& msg) {
  std::array<uint8_t, 64> bytes = {0};
  std::array<uint32_t, 16> input = {0};
  {
    std::size_t i{0};
    for (; i < msg.length(); ++i) {
      bytes[i] = msg[i];
    }
    bytes[i] = 0x80;
    const auto bit_count = 8 * msg.length();
    for (i = 56; i < bytes.size(); ++i) {
      bytes[i] = (bit_count >> (8 * (i - 56))) & 0xff;
    }
  }
  for (std::size_t i{0}; i < input.size(); ++i) {
    for (std::size_t j{0}; j < 4; ++j) {
      input[i] |= bytes[4 * i + j] << (8 * j);
    }
  }
  return input;
}

uint32_t partial_md5(const std::string& msg) {
  if (msg.length() > 64 - 8 - 1) {
    throw std::runtime_error("too long message");
  }

  static constexpr std::array<uint32_t, 16> md5_rotations =
      {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};

  static std::array<uint32_t, 64> md5_sine_table = {0};
  if (md5_sine_table.front() == 0) {
    for (std::size_t i{0}; i < md5_sine_table.size(); ++i) {
      const auto x{(1LL << 32) * std::abs(std::sin(i + 1))};
      md5_sine_table[i] = static_cast<uint32_t>(x);
    }
  }

  uint32_t a0 = std::byteswap(0x01234567);
  uint32_t b0 = std::byteswap(0x89abcdef);
  uint32_t c0 = std::byteswap(0xfedcba98);
  uint32_t d0 = std::byteswap(0x76543210);

  const std::array<uint32_t, 16> input = pack_md5_input(msg);
  auto a = a0;
  auto b = b0;
  auto c = c0;
  auto d = d0;

  for (uint32_t i{0}; i < 64; ++i) {
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

int main() {
  std::ios_base::sync_with_stdio(false);

  std::string input;
  std::cin >> input;

  auto find_next = [i = 0, &input](const auto num_zeros) mutable {
    for (;; ++i) {
      std::ostringstream buf(input, std::ios_base::ate);
      buf << i;
      const auto res = partial_md5(buf.str());
      if ((res >> (32 - 4 * num_zeros)) == 0) {
        return i;
      }
    }
  };

  const auto part1{find_next(5)};
  const auto part2{find_next(6)};
  std::cout << part1 << ' ' << part2 << '\n';

  return 0;
}
