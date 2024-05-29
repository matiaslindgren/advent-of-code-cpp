#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using Vec3 = ndvec::vec3<long>;

int wrap(int x, int n) {
  return ((x % n) + n) % n;
}

struct Blizzard {
  Vec2 pos;
  Vec2 dir;

  auto operator<=>(const Blizzard&) const = default;
};

struct Grid {
  std::vector<Blizzard> blizzards;
  int width{};
  int height{};

  [[nodiscard]]
  bool is_inside(Vec2 p) const {
    return 0 <= p.x() and p.x() < width and 0 <= p.y() and p.y() < height;
  }
};

auto simulate_until_cycle(Grid grid) {
  std::vector<std::vector<Blizzard>> states;
  for (auto state{grid.blizzards}; states.empty() or not ranges::equal(state, states.front());) {
    if (states.size() > 10'000) {
      throw std::runtime_error("failed to find a cycle in reasonable amount of steps");
    }
    states.push_back(state);
    for (Blizzard& b : state) {
      b.pos += b.dir;
      b.pos.x() = wrap(b.pos.x(), grid.width);
      b.pos.y() = wrap(b.pos.y(), grid.height);
    }
    ranges::sort(state, {}, [](const Blizzard& b) {
      return std::tuple{b.pos.y(), b.pos.x(), b.dir.y(), b.dir.x()};
    });
  }
  return states;
}

auto find_shortest_path(
    const Grid& grid,
    const Vec2 src,
    const Vec2 dst,
    const auto n_states,
    const auto& blizzards,
    const long init_time = 0
) {
  std::unordered_set<Vec3> visited;
  for (std::deque q{std::pair{src, init_time}}; not q.empty(); q.pop_front()) {
    auto [pos, time]{q.front()};
    if (pos == dst) {
      return time;
    }
    if (pos != src and not grid.is_inside(pos)) {
      continue;
    }
    if (blizzards.contains(Vec3(pos.x(), pos.y(), time % n_states))) {
      continue;
    }
    if (auto&& [_, is_new]{visited.insert(Vec3(pos.x(), pos.y(), time))}; is_new) {
      for (Vec2 adj : pos.adjacent()) {
        q.emplace_back(adj, time + 1);
      }
      q.emplace_back(pos, time + 1);
    }
  }
  throw std::runtime_error(std::format("no path from {} to {}", src, dst));
}

auto search(Grid grid, const Vec2 begin, const Vec2 end) {
  std::unordered_set<Vec3> blizzards;
  const auto states{simulate_until_cycle(grid)};
  for (auto [z, state] : my_std::views::enumerate(states)) {
    for (const Blizzard& b : state) {
      blizzards.insert(Vec3(b.pos.x(), b.pos.y(), z));
    }
  }
  auto part1{find_shortest_path(grid, begin, end, states.size(), blizzards)};
  auto part2_half{find_shortest_path(grid, end, begin, states.size(), blizzards, part1)};
  auto part2{find_shortest_path(grid, begin, end, states.size(), blizzards, part2_half)};
  return std::pair{part1, part2};
}

auto parse_input(std::string_view path) {
  Grid grid;
  std::optional<Vec2> begin;
  std::optional<Vec2> end;

  const auto parse_blizzard{[&](const Vec2& p, char ch) {
    if (ch == '.') {
      if (not begin) {
        begin = p - Vec2(1, 1);
      }
      end = p - Vec2(1, 1);
    } else if (ch == '>') {
      grid.blizzards.emplace_back(p - Vec2(1, 1), Vec2(1, 0));
    } else if (ch == 'v') {
      grid.blizzards.emplace_back(p - Vec2(1, 1), Vec2(0, 1));
    } else if (ch == '<') {
      grid.blizzards.emplace_back(p - Vec2(1, 1), Vec2(-1, 0));
    } else if (ch == '^') {
      grid.blizzards.emplace_back(p - Vec2(1, 1), Vec2(0, -1));
    } else {
      throw std::runtime_error(std::format("unknown grid tile '{}'", ch));
    }
  }};

  const auto lines{aoc::slurp_lines(path)};
  if (lines.size() < 3) {
    throw std::runtime_error("input must contain at least 3 rows");
  }

  for (Vec2 p; const std::string& line : lines) {
    if (line.size() < 3) {
      throw std::runtime_error("input must contain at least 3 columns");
    }
    for (p.x() = 0; char ch : line) {
      if (ch == '#') {
        if (0 < p.x() and p.x() < line.size() - 1 and 0 < p.y() and p.y() < lines.size() - 1) {
          throw std::runtime_error(std::format("a non-border wall '#' at {} is out of place", p));
        }
      } else {
        parse_blizzard(p, ch);
      }
      p.x() += 1;
    }
    if (grid.width == 0) {
      grid.width = p.x() - 2;
    } else if (grid.width != p.x() - 2) {
      throw std::runtime_error("every line must be of equal length");
    }
    p.y() += 1;
    grid.height += 1;
  }

  grid.height -= 2;

  return std::tuple{grid, begin.value(), end.value()};
}

int main() {
  const auto [grid, begin, end]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(grid, begin, end)};
  std::println("{} {}", part1, part2);
  return 0;
}
