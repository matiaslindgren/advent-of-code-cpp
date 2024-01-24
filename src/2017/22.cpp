import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Vec2 {
  int y{}, x{};

  Vec2 rotate_right() const {
    return {x, -y};
  }
  Vec2 rotate_left() const {
    return {-x, y};
  }
  Vec2& operator+=(const Vec2& rhs) {
    y += rhs.y;
    x += rhs.x;
    return *this;
  }
  auto operator<=>(const Vec2&) const = default;
};

template <>
struct std::hash<Vec2> {
  static constexpr auto max_half{std::numeric_limits<int>::max() / 2};
  static constexpr auto width{std::numeric_limits<std::size_t>::digits / 2};
  std::size_t operator()(const Vec2& v) const noexcept {
    const auto y{static_cast<std::size_t>(v.y + max_half)};
    const auto x{static_cast<std::size_t>(v.x + max_half)};
    return (y << width) | x;
  }
};

enum struct State {
  weakened,
  infected,
  flagged,
};

auto parse_grid(const auto& lines) {
  std::unordered_map<Vec2, State> grid;
  const int row_count = lines.size();
  const int col_count = lines.front().size();
  for (int row{}; row < row_count; ++row) {
    for (int col{}; col < col_count; ++col) {
      if (lines[row][col] == '#') {
        grid[Vec2{row, col}] = State::infected;
      }
    }
  }
  const Vec2 begin{row_count / 2, col_count / 2};
  return std::pair{grid, begin};
}

auto count_part1(auto grid, Vec2 virus, const auto bursts) {
  int count{};
  Vec2 dir{-1, 0};
  for (int b{}; b < bursts; ++b) {
    if (grid.contains(virus)) {
      grid.erase(virus);
      dir = dir.rotate_right();
    } else {
      grid[virus] = State::infected;
      ++count;
      dir = dir.rotate_left();
    }
    virus += dir;
  }
  return count;
}

auto count_part2(auto grid, Vec2 virus, const auto bursts) {
  int count{};
  Vec2 dir{-1, 0};
  for (int b{}; b < bursts; ++b) {
    if (grid.contains(virus)) {
      auto& state{grid.at(virus)};
      switch (state) {
        case State::weakened: {
          ++count;
          state = State::infected;
        } break;
        case State::infected: {
          dir = dir.rotate_right();
          state = State::flagged;
        } break;
        case State::flagged: {
          dir = dir.rotate_right().rotate_right();
          grid.erase(virus);
        } break;
      }
    } else {
      grid[virus] = State::weakened;
      dir = dir.rotate_left();
    }
    virus += dir;
  }
  return count;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto lines{views::istream<std::string>(input) | ranges::to<std::vector>()};
  const auto [grid, begin] = parse_grid(lines);

  const auto part1{count_part1(grid, begin, 10000)};
  const auto part2{count_part2(grid, begin, 10000000)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
