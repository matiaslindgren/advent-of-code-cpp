import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

auto search(const auto& items) {
  long part1{}, part2{};
  for (auto [i, x] : my_std::views::enumerate(items)) {
    for (auto [j, y] : my_std::views::enumerate(views::drop(items, i + 1))) {
      if (x + y == 2020) {
        part1 = x * y;
      }
      for (auto z : views::drop(items, i + j + 2)) {
        if (x + y + z == 2020) {
          part2 = x * y * z;
        }
        if (part1 and part2) {
          return std::pair{part1, part2};
        }
      }
    }
  }
  throw std::runtime_error("could not find answer");
}

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  auto items{views::istream<int>(is) | ranges::to<std::vector>()};
  if (is.eof()) {
    return items;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto items{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(items)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
