import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

enum struct State {
  weakened,
  infected,
  flagged,
};

using Vec2 = aoc::Vec2<int>;

auto parse_grid(const auto& lines) {
  std::unordered_map<Vec2, State> grid;
  const int row_count = lines.size();
  const int col_count = lines.front().size();
  for (int row{}; row < row_count; ++row) {
    for (int col{}; col < col_count; ++col) {
      if (lines[row][col] == '#') {
        grid[Vec2(col, row)] = State::infected;
      }
    }
  }
  Vec2 begin(col_count / 2, row_count / 2);
  return std::pair{grid, begin};
}

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

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto lines{views::istream<std::string>(input) | ranges::to<std::vector>()};
  const auto [grid, begin] = parse_grid(lines);

  const auto part1{count_part1(grid, begin, 10'000)};
  const auto part2{count_part2(grid, begin, 10'000'000)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
