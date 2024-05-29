#include "aoc.hpp"
#include "intcode.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using intcode::IntCode;
using Vec2 = ndvec::vec2<int>;

auto explore(const auto& program) {
  std::unordered_set<Vec2> tiles;
  std::unordered_set<Vec2> visited;
  Vec2 oxygen;
  int oxygen_distance{};

  constexpr std::array adjacent{
      Vec2(0, -1),
      Vec2(0, 1),
      Vec2(-1, 0),
      Vec2(1, 0),
  };

  for (std::deque q{std::tuple{IntCode(program), Vec2(), 0}}; not q.empty(); q.pop_front()) {
    auto [droid_prev, pos_prev, n_moves]{q.front()};
    tiles.insert(pos_prev);
    for (int cmd{1}; cmd <= 4; ++cmd) {
      Vec2 step{adjacent.at(cmd - 1)};
      Vec2 pos{pos_prev + step};
      if (auto [_, unseen]{visited.insert(pos)}; not unseen) {
        continue;
      }

      IntCode droid(droid_prev);
      droid.input.push_back(cmd);
      switch (droid.run_until_output().value()) {
        case 2: {
          oxygen_distance = n_moves + 1;
          oxygen = pos;
          [[fallthrough]];
        }
        case 1: {
          q.emplace_back(droid, pos, n_moves + 1);
        } break;
        case 0: {
        } break;
        default: {
          throw std::runtime_error("unexpected status from droid");
        }
      }
    }
  }

  return std::tuple{oxygen_distance, oxygen, tiles};
}

auto flood_fill(Vec2 oxygen, const auto& tiles) {
  int max_moves{};
  {
    std::unordered_set<Vec2> visited;
    for (std::deque q{std::pair{oxygen, 0}}; not q.empty(); q.pop_front()) {
      auto [pos, n_moves]{q.front()};
      if (not tiles.contains(pos)) {
        continue;
      }
      if (auto [_, is_new]{visited.insert(pos)}; is_new) {
        max_moves = std::max(max_moves, n_moves);
        for (Vec2 adj : pos.adjacent()) {
          q.emplace_back(adj, n_moves + 1);
        }
      }
    }
  }
  return max_moves;
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto [part1, oxygen, tiles]{explore(program)};
  const auto part2{flood_fill(oxygen, tiles)};

  std::println("{} {}", part1, part2);

  return 0;
}
