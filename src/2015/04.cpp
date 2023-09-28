import std;

// References (accessed 2023-09-28)
// 1. https://datatracker.ietf.org/doc/html/rfc1321
// 2. https://en.wikipedia.org/wiki/MD5

constexpr std::array<uint32_t, 16> md5_rotations =
    {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};
constexpr std::array<uint32_t, 64> md5_sine_table = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

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

  std::size_t part1{0};
  std::size_t part2{0};
  for (std::size_t i{1}; !(part1 && part2); ++i) {
    std::ostringstream buf(input, std::ios_base::ate);
    buf << i;
    const uint32_t res = partial_md5(buf.str());
    if (!part1 && (res >> (32 - 20)) == 0) {
      part1 = i;
    }
    if (!part2 && (res >> (32 - 24)) == 0) {
      part2 = i;
    }
  }

  std::cout << part1 << ' ' << part2 << '\n';

  return 0;
}
