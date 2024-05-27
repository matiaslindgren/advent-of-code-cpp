#include "ndvec.hpp"
#include "std.hpp"

using Vec2 = ndvec::vec2<int>;

auto search(int n) {
  std::unordered_map<Vec2, int> grid;

  const auto adjacent_sum{[&grid](const Vec2& p) {
    int sum{};
    for (Vec2 d(-1, -1); d.x() <= 1; d.x() += 1) {
      for (d.y() = -1; d.y() <= 1; d.y() += 1) {
        if (d != Vec2()) {
          sum += grid[p + d];
        }
      }
    }
    return sum;
  }};

  int part1{};
  int part2{};

  for (auto [n_steps, len, p, d]{std::tuple{1, 1, Vec2(), Vec2(1, 0)}}; std::min(part1, part2) == 0;
       len += 1) {
    for (int turn{}; turn < 2; ++turn) {
      for (int step{}; step < len; ++step) {
        const auto sum{adjacent_sum(p)};
        if (part2 == 0 and sum > n) {
          part2 = sum;
        }
        grid[p] = std::max(1, sum);
        p += d;
        n_steps += 1;
        if (n_steps == n) {
          part1 = p.abs().sum();
        }
      }
      d.rotate_left();
    }
  }

  return std::pair{part1, part2};
}

int main() {
  std::ios::sync_with_stdio(false);
  if (int n{}; std::cin >> n and n > 0) {
    const auto [part1, part2]{search(n)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input, it should be a single positive integer");
}
