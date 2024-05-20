#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

auto parse_input(std::string path) {
  using aoc::skip;
  using std::operator""s;

  std::istringstream is{aoc::slurp_file(path)};
  if (int depth; is >> skip("depth:"s) >> depth) {
    if (int x, y; is >> std::ws >> skip("target:"s) >> x >> skip(","s) >> y) {
      if (is or is.eof()) {
        return std::pair{depth, Vec2(x, y)};
      }
    }
  }

  throw std::runtime_error("failed parsing input");
}

auto get_erosion(const auto depth, const Vec2& target, const Vec2& p) {
  static std::unordered_map<Vec2, int> erosion;
  if (auto it{erosion.find(p)}; it != erosion.end()) {
    return it->second;
  }
  long index;
  if ((p.x() == 0 and p.y() == 0) or p == target) {
    index = 0;
  } else if (p.x() == 0) {
    index = p.y() * 48'271;
  } else if (p.y() == 0) {
    index = p.x() * 16'807;
  } else {
    index = get_erosion(depth, target, p - Vec2(0, 1)) * get_erosion(depth, target, p - Vec2(1, 0));
  }
  return (erosion[p] = (index + depth) % 20183);
}

auto find_part1(const auto depth, const Vec2 target) {
  int risk{};
  for (Vec2 p{}; p.y() <= target.y(); ++p.y()) {
    for (p.x() = 0; p.x() <= target.x(); ++p.x()) {
      risk += get_erosion(depth, target, p) % 3;
    }
  }
  return risk;
}

struct State {
  Vec2 pos{};
  int tool{};
  auto operator<=>(const State&) const = default;
};

template <>
struct std::hash<State> {
  constexpr std::size_t operator()(const State& s) const noexcept {
    return (std::hash<Vec2>{}(s.pos) << 2) | s.tool;
  }
};

auto find_part2(const auto depth, const Vec2 target) {
  // Dijkstra
  std::unordered_map<State, int> time;
  const auto time_to_reach{[&time](const State& s) {
    if (const auto it{time.find(s)}; it != time.end()) {
      return it->second;
    }
    return std::numeric_limits<int>::max();
  }};

  std::vector<State> q;
  const auto push_min_time_heap{[&](State s) {
    q.push_back(s);
    ranges::push_heap(q, ranges::greater{}, time_to_reach);
  }};

  const auto pop_min_time_heap{[&] {
    ranges::pop_heap(q, ranges::greater{}, time_to_reach);
    auto state{q.back()};
    q.pop_back();
    return state;
  }};

  const State begin{}, end{.pos = target};
  q.push_back(begin);
  time[begin] = 0;

  for (std::unordered_set<State> visited; not q.empty();) {
    State s{pop_min_time_heap()};
    if (s == end) {
      break;
    }
    if (s.pos.y() < 0 or s.pos.x() < 0) {
      continue;
    }
    if (auto&& [_, unseen]{visited.insert(s)}; not unseen) {
      continue;
    }
    if ((get_erosion(depth, target, s.pos) + 2) % 3 == s.tool) {
      // rocky  0 : torch 0 or climb 1 => not empty 2
      // wet    1 : climb 1 or empty 2 => not torch 0
      // narrow 2 : empty 2 or torch 0 => not climb 1
      continue;
    }
    for (auto adj : s.pos.adjacent()) {
      for (auto&& [pos, tool, t] : std::array{
               std::tuple{s.pos, (s.tool + 1) % 3, 7},
               std::tuple{s.pos, (s.tool + 2) % 3, 7},
               std::tuple{adj, s.tool, 1},
           }) {
        State dst{pos, tool};
        const auto new_time{aoc::saturating_add(time_to_reach(s), t)};
        if (new_time < time_to_reach(dst)) {
          time[dst] = new_time;
          push_min_time_heap(dst);
        }
      }
    }
  }

  return time_to_reach(end);
}

int main() {
  const auto [depth, target]{parse_input("/dev/stdin")};

  const auto part1{find_part1(depth, target)};
  const auto part2{find_part2(depth, target)};

  std::println("{} {}", part1, part2);

  return 0;
}
