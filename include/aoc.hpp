#ifndef AOC_HEADER_INCLUDED
#define AOC_HEADER_INCLUDED

#include "std.hpp"

using std::operator""sv;

namespace aoc {

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

std::vector<std::string> slurp_lines(std::string_view path) {
  std::vector<std::string> lines;
  {
    std::istringstream is{slurp_file(path)};
    for (std::string line; std::getline(is, line);) {
      lines.push_back(line);
    }
    if (not is and not is.eof()) {
      throw std::runtime_error("input is invalid");
    }
    return lines;
  }
}

template <typename T>
std::vector<T> slurp(std::string_view path) {
  std::istringstream is{slurp_file(path)};
  auto items{std::views::istream<T>(is) | std::ranges::to<std::vector>()};
  if (items.empty()) {
    throw std::runtime_error("input is empty");
  }
  if (not is and not is.eof()) {
    throw std::runtime_error("input is invalid");
  }
  return items;
}

auto cpu_count() {
  return std::max(1u, std::thread::hardware_concurrency());
};

template <typename... Patterns>
class skip {
  std::tuple<Patterns...> patterns;

  constexpr std::istream& skip_pattern(std::istream& is, auto&& p) const {
    if (is) {
      for (char rhs : p) {
        if (char lhs; not(is.get(lhs) and lhs == rhs)) {
          is.setstate(std::ios_base::failbit);
          break;
        }
      }
    }
    return is;
  }

  constexpr std::istream& skip_pattern(std::istream& is, auto&& p, auto&&... ps) const {
    if (skip_pattern(is, p)) {
      return (skip_pattern(is >> std::ws, ps), ...);
    }
    return is;
  }

 public:
  constexpr skip(Patterns&&... ps) : patterns(std::move(ps)...) {
  }

  constexpr std::istream& consume(std::istream& is) const {
    return std::apply(
        [&](auto&&... ps) -> std::istream& { return skip_pattern(is, ps...); },
        patterns
    );
  }
};

template <typename... Patterns>
std::istream& operator>>(std::istream& is, skip<Patterns...>&& s) {
  return s.consume(is);
}

template <std::integral Int>
constexpr Int saturating_add(Int a, Int b) {
  // https://stackoverflow.com/a/17582366/5951112
  // (2024-03-12)
  constexpr Int intmin{std::numeric_limits<Int>::min()};
  constexpr Int intmax{std::numeric_limits<Int>::max()};
  if (a > 0) {
    if (b > intmax - a) {
      return intmax;
    }
  } else if (b < intmin - a) {
    return intmin;
  }
  return a + b;
}

constexpr std::array ocr_letter_rows{
    // 6-pixel character map adapted from
    // https://github.com/bsoyka/advent-of-code-ocr/blob/aae11d40720f0b681f2684b7aa4d25e3e0956b11/advent_of_code_ocr/characters.py
    // Accessed 2023-11-20
    std::pair{".##.#..##..######..##..#"sv, 'A'},
    std::pair{"###.#..####.#..##..####."sv, 'B'},
    std::pair{".##.#..##...#...#..#.##."sv, 'C'},
    std::pair{"#####...###.#...#...####"sv, 'E'},
    std::pair{"#####...###.#...#...#..."sv, 'F'},
    std::pair{".##.#..##...#.###..#.###"sv, 'G'},
    std::pair{"#..##..######..##..##..#"sv, 'H'},
    std::pair{".###..#...#...#...#..###"sv, 'I'},
    std::pair{"..##...#...#...##..#.##."sv, 'J'},
    std::pair{"#..##.#.##..#.#.#.#.#..#"sv, 'K'},
    std::pair{"#...#...#...#...#...####"sv, 'L'},
    std::pair{".##.#..##..##..##..#.##."sv, 'O'},
    std::pair{"###.#..##..####.#...#..."sv, 'P'},
    std::pair{"###.#..##..####.#.#.#..#"sv, 'R'},
    std::pair{".####...#....##....####."sv, 'S'},
    std::pair{"#..##..##..##..##..#.##."sv, 'U'},
    std::pair{"#...#....#.#..#...#...#."sv, 'Y'},
    std::pair{"####...#..#..#..#...####"sv, 'Z'},
    // 10-pixel character map adapted from
    // https://gist.github.com/usbpc/5fa0be48ad7b4b0594b3b8b029bc47b4
    // Accessed 2024-01-29
    std::pair{"..##...#..#.#....##....##....########....##....##....##....#"sv, 'A'},
    std::pair{"#####.#....##....##....######.#....##....##....##....######."sv, 'B'},
    std::pair{".####.#....##.....#.....#.....#.....#.....#.....#....#.####."sv, 'C'},
    std::pair{"#######.....#.....#.....#####.#.....#.....#.....#.....######"sv, 'E'},
    std::pair{"#######.....#.....#.....#####.#.....#.....#.....#.....#....."sv, 'F'},
    std::pair{".####.#....##.....#.....#.....#..####....##....##...##.###.#"sv, 'G'},
    std::pair{"#....##....##....##....########....##....##....##....##....#"sv, 'H'},
    std::pair{"...###....#.....#.....#.....#.....#.....#.#...#.#...#..###.."sv, 'J'},
    std::pair{"#....##...#.#..#..#.#...##....##....#.#...#..#..#...#.#....#"sv, 'K'},
    std::pair{"#.....#.....#.....#.....#.....#.....#.....#.....#.....######"sv, 'L'},
    std::pair{"#....###...###...##.#..##.#..##..#.##..#.##...###...###....#"sv, 'N'},
    std::pair{"#####.#....##....##....######.#.....#.....#.....#.....#....."sv, 'P'},
    std::pair{"#####.#....##....##....######.#..#..#...#.#...#.#....##....#"sv, 'R'},
    std::pair{"#....##....#.#..#..#..#...##....##...#..#..#..#.#....##....#"sv, 'X'},
    std::pair{"######.....#.....#....#....#....#....#....#.....#.....######"sv, 'Z'},
};

constexpr char ocr(std::string_view rows) {
  if (const auto it{
          std::ranges::find_if(ocr_letter_rows, [&rows](auto&& p) { return p.first == rows; })
      };
      it != ocr_letter_rows.end()) {
    return it->second;
  }
  return ' ';
}

}  // namespace aoc

#endif // AOC_HEADER_INCLUDED
