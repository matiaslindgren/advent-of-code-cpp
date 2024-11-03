#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& games) {
  return sum(views::transform(games, [](auto g) {
    auto [lhs, rhs]{g};
    const int lhs_loses{lhs % 3 + 1};
    return rhs + 3 * (lhs == rhs) + 6 * (rhs == lhs_loses);
  }));
}

auto find_part2(const auto& games) {
  return sum(views::transform(games, [](auto g) {
    auto [lhs, rhs]{g};
    const int lhs_loses{lhs % 3 + 1};
    const int lhs_wins{(lhs + 1) % 3 + 1};
    return (rhs == 1) * lhs_wins + (rhs == 2) * (3 + lhs) + (rhs == 3) * (6 + lhs_loses);
  }));
}

int encode_hand(char ch) {
  switch (ch) {
    case 'A':
    case 'X':
      return 1;
    case 'B':
    case 'Y':
      return 2;
    case 'C':
    case 'Z':
      return 3;
  }
  throw std::runtime_error(std::format("unknown hand '{}'", ch));
}

auto parse_games(std::string_view path) {
  std::vector<std::pair<int, int>> games;
  {
    const auto lines{aoc::slurp_lines(path)};
    for (auto line : lines) {
      if (line.size() == 3) {
        games.emplace_back(encode_hand(line[0]), encode_hand(line[2]));
      } else {
        throw std::runtime_error("every line must contain 3 characters");
      }
    }
  }
  if (games.empty()) {
    throw std::runtime_error("empty input");
  }
  return games;
}

int main() {
  const auto games{parse_games("/dev/stdin")};

  const auto part1{find_part1(games)};
  const auto part2{find_part2(games)};

  std::println("{} {}", part1, part2);

  return 0;
}
