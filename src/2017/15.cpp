import std;
import aoc;

auto parse_input(std::istream& is) {
  using aoc::skip;
  using std::operator""s;
  if (int a; is >> skip("Generator"s, "A"s, "starts"s, "with"s) >> a) {
    if (int b; is >> std::ws >> skip("Generator"s, "B"s, "starts"s, "with"s) >> b) {
      return std::pair{a, b};
    }
  }
  throw std::runtime_error("failed parsing generator starting values");
}

constexpr auto limit{(1u << 31) - 1u};

auto next_a(const auto a) {
  return (a * 16807u) % limit;
}

auto next_b(const auto b) {
  return (b * 48271u) % limit;
}

bool is_match(const auto a, const auto b) {
  return (a & 0xffff) == (b & 0xffff);
}

int find_part1(std::size_t a, std::size_t b) {
  int matches{};
  for (auto i{0uz}; i < 40'000'000; ++i) {
    a = next_a(a);
    b = next_b(b);
    matches += is_match(a, b);
  }
  return matches;
}

int find_part2(std::size_t a, std::size_t b) {
  int matches{};
  for (auto i{0uz}; i < 5'000'000; ++i) {
    while ((a = next_a(a)) % 4) {
    }
    while ((b = next_b(b)) % 8) {
    }
    matches += is_match(a, b);
  }
  return matches;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto [a, b] = parse_input(input);

  const auto part1{find_part1(a, b)};
  const auto part2{find_part2(a, b)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
