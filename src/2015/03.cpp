import std;

// TODO(llvm17)
namespace my_ranges {
template <class Container>
  requires(!std::ranges::view<Container>)
constexpr auto stride(std::size_t n) {
  return std::__range_adaptor_closure_t([=]<std::ranges::input_range R>(R &&r) {
    Container c;
    std::size_t i{0};
    for (const auto &x : r) {
      if (i++ % n == 0) {
        c.push_back(x);
      }
    }
    return c;
  });
}
}  // namespace my_ranges

enum struct Direction : char {
  north = '^',
  east = '>',
  south = 'v',
  west = '<',
};

std::istream &operator>>(std::istream &is, Direction &d) {
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
  is.setstate(std::ios_base::failbit);
  return is;
}

auto count_visited_houses(const auto &...instructions_list) {
  std::unordered_map<long, int> visit_counts;
  const auto deliver_presents = [&visit_counts](const auto &instructions) {
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
  return std::ranges::count_if(std::views::values(visit_counts),
                               [](const auto &n) { return n > 0; });
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto instructions = std::views::istream<Direction>(std::cin) |
                            my_ranges::stride<std::vector<Direction>>(1);

  const auto part1 = count_visited_houses(instructions);

  // TODO(llvm17)
#if 0
  const auto santa_instructions = instructions | std::views::stride(2) |
                                std::ranges::to<std::vector<Direction>>();
  const auto robot_instructions = instructions | std::views::drop(1) |
                                std::views::stride(2) |
                                std::ranges::to<std::vector<Direction>>();
#endif

  const auto santa_instructions =
      instructions | my_ranges::stride<std::vector<Direction>>(2);
  const auto robot_instructions = instructions | std::views::drop(1) |
                                  my_ranges::stride<std::vector<Direction>>(2);
  const auto part2 =
      count_visited_houses(santa_instructions, robot_instructions);

  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
