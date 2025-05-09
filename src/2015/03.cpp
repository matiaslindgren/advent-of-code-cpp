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

std::istream& operator>>(std::istream& is, Direction& dir) {
  if (std::underlying_type_t<Direction> ch{}; is >> ch) {
    switch (ch) {
      case std::to_underlying(Direction::north):
      case std::to_underlying(Direction::east):
      case std::to_underlying(Direction::south):
      case std::to_underlying(Direction::west): {
        dir = {ch};
      } break;
      default: {
        throw std::runtime_error(std::format("invalid direction '{}'", ch));
      }
    }
  }
  return is;
}

using Moves = std::vector<Direction>;

int count_visited_houses(const auto&... moves_list) {
  std::unordered_map<long, int> visit_counts;
  const auto deliver_presents{[&visit_counts](const auto& moves) {
    const auto grid_size{moves.size()};
    visit_counts[0] += 1;
    int x{};
    int y{};
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
      visit_counts[y + grid_size * x] += 1;
    }
  }};
  (deliver_presents(moves_list), ...);
  return ranges::count_if(views::values(visit_counts), [](auto n) { return n > 0; });
}

int main() {
  const auto all_moves{aoc::parse_items<Direction>("/dev/stdin")};

  // TODO(llvm21?) P1899R3 std::views::stride
  const auto santa_moves{all_moves | my_std::views::stride(2) | ranges::to<Moves>()};
  // TODO(llvm21?) P1899R3 std::views::stride
  const auto robot_moves{
      all_moves | views::drop(1) | my_std::views::stride(2) | ranges::to<Moves>()
  };

  const auto part1{count_visited_houses(all_moves)};
  const auto part2{count_visited_houses(santa_moves, robot_moves)};

  std::println("{} {}", part1, part2);

  return 0;
}
