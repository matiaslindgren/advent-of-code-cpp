module;
// Various helpers, independent of each AoC year
import std;

export module aoc;

export namespace aoc {

void init_io() {
  std::ios::sync_with_stdio(false);
  std::cin.exceptions(std::ifstream::badbit);
}

std::string slurp_file(std::string_view path) {
  std::ios::sync_with_stdio(false);
  std::ifstream is{path, std::ios::in | std::ios::binary};
  is.exceptions(std::ifstream::badbit);
  std::string data;
  for (std::array<char, 1024> buffer; is;) {
    is.read(buffer.data(), buffer.size());
    std::ranges::copy(buffer | std::views::take(is.gcount()), std::back_inserter(data));
  }
  return data;
}

auto cpu_count() {
  return std::max(1u, std::thread::hardware_concurrency());
};

std::istream& skip(std::istream& is, auto s) {
  for (char rhs : s) {
    if (char lhs; !(is.get(lhs) && lhs == rhs)) {
      is.setstate(std::ios_base::failbit);
      return is;
    }
  }
  return is;
}

// Character map adapted from
// https://github.com/bsoyka/advent-of-code-ocr/blob/aae11d40720f0b681f2684b7aa4d25e3e0956b11/advent_of_code_ocr/characters.py
// Accessed 2023-11-20
char as_ascii(std::string aoc_letter) {
  using std::operator""s;
  static const std::unordered_map<std::string, char> charmap = {
      {".##.#..##..######..##..#"s, 'A'},
      {"###.#..####.#..##..####."s, 'B'},
      {".##.#..##...#...#..#.##."s, 'C'},
      {"#####...###.#...#...####"s, 'E'},
      {"#####...###.#...#...#..."s, 'F'},
      {".##.#..##...#.###..#.###"s, 'G'},
      {"#..##..######..##..##..#"s, 'H'},
      {".###..#...#...#...#..###"s, 'I'},
      {"..##...#...#...##..#.##."s, 'J'},
      {"#..##.#.##..#.#.#.#.#..#"s, 'K'},
      {"#...#...#...#...#...####"s, 'L'},
      {".##.#..##..##..##..#.##."s, 'O'},
      {"###.#..##..####.#...#..."s, 'P'},
      {"###.#..##..####.#.#.#..#"s, 'R'},
      {".####...#....##....####."s, 'S'},
      {"#..##..##..##..##..#.##."s, 'U'},
      {"#...#....#.#..#...#...#."s, 'Y'},
      {"####...#..#..#..#...####"s, 'Z'},
  };
  if (const auto it{charmap.find(aoc_letter)}; it != charmap.end()) {
    return it->second;
  }
  return ' ';
}
}  // namespace aoc
