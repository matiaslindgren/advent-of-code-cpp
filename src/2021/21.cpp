#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

using UInt = unsigned long;

struct State {
  int pawn1{}, pawn2{};
  UInt score1{}, score2{};
  int turn{1};

  State roll(int die) const {
    State s{*this};
    if (turn == 1) {
      s.pawn1 = (pawn1 + die + 9) % 10 + 1;
    } else {
      s.pawn2 = (pawn2 + die + 9) % 10 + 1;
    }
    return s;
  }

  State next_turn() const {
    State s{*this};
    if (turn == 1) {
      s.score1 += pawn1;
    } else {
      s.score2 += pawn2;
    }
    s.turn = s.turn % 2 + 1;
    return s;
  }

  auto operator<=>(const State&) const = default;
};

template <>
struct std::hash<State> {
  std::size_t operator()(const State& s) const noexcept {
    auto h{std::hash<int>{}(s.turn)};
    h = (h << 2) | std::hash<int>{}(s.pawn1);
    h = (h << 4) | std::hash<int>{}(s.pawn2);
    h = (h << 4) | std::hash<UInt>{}(s.score1);
    h = (h << 27) | std::hash<UInt>{}(s.score1);
    return h;
  }
};

auto find_part1(State s) {
  for (int rolls{};;) {
    for (int roll{}; roll < 3; ++roll) {
      ++rolls;
      s = s.roll(rolls);
    }
    s = s.next_turn();
    if (s.score1 >= 1000) {
      return rolls * s.score2;
    }
    if (s.score2 >= 1000) {
      return rolls * s.score1;
    }
  }
}

using WinCounts = std::pair<UInt, UInt>;

UInt find_part2(State init_state) {
  auto count_wins{
      [cache = std::unordered_map<State, WinCounts>{}](this auto&& self, State s1) -> WinCounts {
        if (cache.contains(s1)) {
          return cache.at(s1);
        }
        if (s1.score1 >= 21) {
          return {1, 0};
        }
        if (s1.score2 >= 21) {
          return {0, 1};
        }
        std::vector<State> states{{s1}};
        for (int throw_num{}; throw_num < 3; ++throw_num) {
          for (State s2 : std::exchange(states, {})) {
            for (int die{1}; die <= 3; ++die) {
              states.push_back(s2.roll(die));
            }
          }
        }
        WinCounts wins{};
        for (State s2 : states) {
          auto w{self(s2.next_turn())};
          wins.first += w.first;
          wins.second += w.second;
        }
        return (cache[s1] = wins);
      }
  };
  return std::apply(ranges::max, count_wins(init_state));
}

auto parse_players(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int p1; is >> skip("Player 1 starting position:"s) >> p1 >> std::ws) {
    if (int p2; is >> skip("Player 2 starting position:"s) >> p2 >> std::ws) {
      if (is.eof()) {
        return std::pair{p1, p2};
      }
    }
  }
  throw std::runtime_error("failed parsing players");
}

int main() {
  const auto [p1, p2]{parse_players("/dev/stdin")};
  State state{.pawn1 = p1, .pawn2 = p2};

  const auto part1{find_part1(state)};
  const auto part2{find_part2(state)};

  std::println("{} {}", part1, part2);

  return 0;
}
