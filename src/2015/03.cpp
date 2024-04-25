#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class Direction : char {
  north = '^',
  east = '>',
  south = 'v',
  west = '<',
};

std::istream& operator>>(std::istream& is, Direction& d) {
  if (std::underlying_type_t<Direction> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Direction::north):
      case std::to_underlying(Direction::east):
      case std::to_underlying(Direction::south):
      case std::to_underlying(Direction::west):
        d = {ch};
        return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Direction");
}

using Moves = std::vector<Direction>;

int count_visited_houses(const auto&... moves_list) {
  std::unordered_map<long, int> visit_counts;
  const auto deliver_presents{[&visit_counts](const auto& moves) {
    const auto grid_size{moves.size()};
    ++visit_counts[0];
    int x{0};
    int y{0};
    for (const auto direction : moves) {
      switch (direction) {
        case Direction::north: {
          --y;
        } break;
        case Direction::east: {
          ++x;
        } break;
        case Direction::south: {
          ++y;
        } break;
        case Direction::west: {
          --x;
        } break;
      }
      ++visit_counts[y + grid_size * x];
    }
  }};
  (deliver_presents(moves_list), ...);
  return ranges::count_if(views::values(visit_counts), [](auto n) { return n > 0; });
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  // clang-format off
  const auto all_moves{
    views::istream<Direction>(input)
    | ranges::to<Moves>()
  };

  // TODO(llvm19?) P1899R3 std::views::stride
  const auto santa_moves{
    all_moves
    | my_std::views::stride(2)
    | ranges::to<Moves>()
  };
  // TODO(llvm19?) P1899R3 std::views::stride
  const auto robot_moves{
    all_moves
    | views::drop(1)
    | my_std::views::stride(2)
    | ranges::to<Moves>()
  };
  // clang-format on

  const auto part1{count_visited_houses(all_moves)};
  const auto part2{count_visited_houses(santa_moves, robot_moves)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
