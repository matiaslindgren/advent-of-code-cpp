import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

int search_divide(const auto& row) {
  for (const auto& a : row) {
    for (const auto& b : row) {
      if (a != b && b && a % b == 0) {
        return a / b;
      }
    }
  }
  throw std::runtime_error("no pair is evenly divisible");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  int part1{}, part2{};

  for (std::string line; std::getline(input, line);) {
    std::istringstream ls{line};
    const auto row{views::istream<int>(ls) | ranges::to<std::vector<int>>()};
    const auto [min, max] = ranges::minmax_element(row);
    part1 += *max - *min;
    part2 += search_divide(row);
  }

  std::print("{} {}\n", part1, part2);

  return 0;
}
