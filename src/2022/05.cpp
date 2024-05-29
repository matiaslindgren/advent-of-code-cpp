#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Move {
  int count{};
  int src{};
  int dst{};
};

enum class Mover : unsigned char {
  v9000,
  v9001,
};

auto search(auto stacks, const auto& moves, Mover version) {
  for (const Move& m : moves) {
    if (m.src >= stacks.size() or m.dst >= stacks.size()) {
      throw std::runtime_error("move out of range, src and dst must be within 1-9");
    }

    auto src{stacks.begin() + m.src};
    auto dst{stacks.begin() + m.dst};

    if (m.count > src->size()) {
      throw std::runtime_error(std::format(
          "cannot take {} crates from src stack with only {} crates",
          m.count,
          src->size()
      ));
    }

    auto pick{*src | views::reverse | views::take(m.count)};
    switch (version) {
      case Mover::v9000: {
        dst->append_range(pick);
      } break;
      case Mover::v9001: {
        dst->append_range(pick | views::reverse);
      } break;
    }

    src->resize(src->size() - m.count);
  }

  return ranges::fold_left(stacks, ""s, [](auto res, auto s) {
    return std::format("{}{}", res, s.empty() ? ' ' : s.back());
  });
}

std::istream& operator>>(std::istream& is, Move& move) {
  if (int count{}, src{}, dst{}; is >> skip("move"s) >> count >> std::ws >> skip("from"s) >> src
                                 >> std::ws >> skip("to"s) >> dst) {
    move = {count, src - 1, dst - 1};
  }
  return is;
}

auto parse_input(std::string_view path) {
  const auto lines{aoc::slurp_lines(path)};
  auto stack_ids{ranges::find(lines, " 1   2   3   4   5   6   7   8   9 "s)};
  if (stack_ids == lines.end()) {
    throw std::runtime_error("stack section should end with a line of stack ids");
  }

  std::vector<std::string> stacks(9);
  for (auto line : ranges::subrange(lines.begin(), stack_ids) | views::reverse) {
    for (auto [s_id, stack] : my_std::views::enumerate(stacks)) {
      if (char ch{line.at(1 + 4 * s_id)}; 'A' <= ch and ch <= 'Z') {
        stack.push_back(ch);
      }
    }
  }

  std::vector<Move> moves;
  for (auto line : ranges::subrange(stack_ids + 1, lines.end())) {
    std::istringstream ls{line};
    if (Move m; ls >> m) {
      moves.push_back(m);
    }
    if (not(ls >> std::ws).eof()) {
      throw std::runtime_error(std::format("failed parsing move: '{}'", line));
    }
  }

  return std::pair{stacks, moves};
}

int main() {
  const auto [stacks, moves]{parse_input("/dev/stdin")};

  const auto part1{search(stacks, moves, Mover::v9000)};
  const auto part2{search(stacks, moves, Mover::v9001)};

  std::println("{} {}", part1, part2);

  return 0;
}
