#include "std.hpp"

int main() {
  std::ios::sync_with_stdio(false);

  int n;
  std::cin >> n;

  std::unordered_map<int, std::unordered_map<int, std::size_t>> grid;

  const auto adjacent_sum{[&grid](int x, int y) {
    auto sum{0uz};
    for (int dx{-1}; dx <= 1; ++dx) {
      for (int dy{-1}; dy <= 1; ++dy) {
        if (not(dx == 0 and dy == 0)) {
          sum += grid[x + dx][y + dy];
        }
      }
    }
    return sum;
  }};

  std::size_t part1{}, part2{};

  int i{1}, x{}, y{}, len{1}, dy{}, dx{1};

  while (not part1 or not part2) {
    for (int k{}; k < 2; ++k) {
      for (int j{}; j < len; ++j) {
        const auto sum{adjacent_sum(x, y)};
        if (not part2 and sum > n) {
          part2 = sum;
        }
        grid[x][y] = std::max(1uz, sum);
        x += dx;
        y += dy;
        if ((i += 1) == n) {
          part1 = std::abs(x) + std::abs(y);
        }
      }
      dy = -std::exchange(dx, dy);
    }
    len += 1;
  }

  std::println("{} {}", part1, part2);

  return 0;
}
