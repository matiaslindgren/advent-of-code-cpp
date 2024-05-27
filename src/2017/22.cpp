#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum struct State : unsigned char {
  weakened,
  infected,
  flagged,
};

auto count_part1(auto grid, Vec2 virus, const auto bursts) {
  int count{};
  Vec2 dir(0, -1);
  for (int b{}; b < bursts; ++b) {
    if (grid.contains(virus)) {
      grid.erase(virus);
      dir.rotate_right();
    } else {
      grid[virus] = State::infected;
      ++count;
      dir.rotate_left();
    }
    virus += dir;
  }
  return count;
}

auto count_part2(auto grid, Vec2 virus, const auto bursts) {
  int count{};
  Vec2 dir(0, -1);
  for (int b{}; b < bursts; ++b) {
    if (grid.contains(virus)) {
      auto& state{grid.at(virus)};
      switch (state) {
        case State::weakened: {
          ++count;
          state = State::infected;
        } break;
        case State::infected: {
          dir.rotate_right();
          state = State::flagged;
        } break;
        case State::flagged: {
          dir.rotate_right();
          dir.rotate_right();
          grid.erase(virus);
        } break;
      }
    } else {
      grid[virus] = State::weakened;
      dir.rotate_left();
    }
    virus += dir;
  }
  return count;
}

auto parse_grid(std::string_view path) {
  std::unordered_map<Vec2, State> grid;
  const auto lines{aoc::parse_items<std::string>(path)};
  for (int row{}; row < lines.size(); ++row) {
    for (int col{}; col < lines.at(row).size(); ++col) {
      if (lines.at(row).at(col) == '#') {
        grid[Vec2(col, row)] = State::infected;
      }
    }
  }
  Vec2 begin{ranges::max(grid | views::keys) / Vec2(2, 2)};
  return std::pair{grid, begin};
}

int main() {
  const auto [grid, begin]{parse_grid("/dev/stdin")};

  const auto part1{count_part1(grid, begin, 10'000)};
  const auto part2{count_part2(grid, begin, 10'000'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
