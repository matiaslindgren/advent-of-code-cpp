import std;
#include "tmp_util.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum struct Direction : char {
  north = '^',
  east = '>',
  south = 'v',
  west = '<',
};

std::istream& operator>>(std::istream& is, Direction& d) {
  std::underlying_type_t<Direction> ch;
  if (is >> ch) {
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

auto count_visited_houses(const auto&... instructions_list) {
  std::unordered_map<long, int> visit_counts;
  const auto deliver_presents = [&visit_counts](const auto& instructions) {
    const auto grid_size = instructions.size();
    ++visit_counts[0];
    int x{0};
    int y{0};
    for (const auto direction : instructions) {
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
  };
  (deliver_presents(instructions_list), ...);
  return ranges::count_if(views::values(visit_counts), [](auto n) { return n > 0; });
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto instructions
      = views::istream<Direction>(std::cin) | my_std::ranges::stride<std::vector<Direction>>(1);
  // TODO(llvm18)
#if 0
  const auto santa_instructions = instructions | views::stride(2) |
                                ranges::to<std::vector<Direction>>();
  const auto robot_instructions = instructions | views::drop(1) |
                                views::stride(2) |
                                ranges::to<std::vector<Direction>>();
#endif
  const auto santa_instructions = instructions | my_std::ranges::stride<std::vector<Direction>>(2);
  const auto robot_instructions
      = instructions | views::drop(1) | my_std::ranges::stride<std::vector<Direction>>(2);

  const auto part1{count_visited_houses(instructions)};
  const auto part2{count_visited_houses(santa_instructions, robot_instructions)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
