module;
// Various helpers, independent of each AoC year
import std;
using std::operator""s;
export module aoc;
export namespace aoc {
// Character map adapted from
// https://github.com/bsoyka/advent-of-code-ocr/blob/aae11d40720f0b681f2684b7aa4d25e3e0956b11/advent_of_code_ocr/characters.py
// Accessed 2023-11-20
char as_ascii(std::string aoc_letter) {
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
