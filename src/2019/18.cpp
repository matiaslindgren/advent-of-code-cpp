#include "std.hpp"
#include "my_std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = aoc::Vec2<int>;
using Graph = std::unordered_map<Vec2, std::unordered_map<Vec2, int>>;

struct Maze {
  std::unordered_map<Vec2, char> keys, doors;
  std::vector<bool> walls;
  std::size_t width{};

  char get_door(const Vec2& p) const {
    if (doors.contains(p)) {
      return doors.at(p);
    }
    return {};
  }

  char get_key(const Vec2& p) const {
    if (keys.contains(p)) {
      return keys.at(p);
    }
    return {};
  }

  auto index(const Vec2& p) const {
    return p.y() * width + p.x();
  }

  Graph as_graph(const Vec2& entrance) const {
    Graph g;
    for (std::vector q{entrance}; not q.empty();) {
      Vec2 src{q.back()};
      q.pop_back();
      for (Vec2 dst : src.adjacent()) {
        if (not walls.at(index(dst)) and not g[src].contains(dst)) {
          g[src][dst] = g[dst][src] = 1;
          q.push_back(dst);
        }
      }
    }
    while (true) {
      const auto redundant{ranges::find_if(g, [&](const auto& edge) {
        auto&& [node, adj]{edge};
        return not get_door(node) and not get_key(node) and node != entrance and adj.size() == 2;
      })};
      if (redundant == g.end()) {
        break;
      }
      const auto [node, adj]{*redundant};
      auto it{adj.begin()};
      const auto [adj1, dist1]{*it};
      const auto [adj2, dist2]{*(++it)};
      g[adj1][adj2] = g[adj2][adj1] = dist1 + dist2;
      g[adj1].erase(node);
      g[adj2].erase(node);
      g.erase(node);
    }
    return g;
  }
};

struct State {
  std::vector<Vec2> robots;
  std::bitset<'z' - 'a' + 1> keys{};

  bool has_key(unsigned char key) const {
    return keys[key - 'a'];
  }
  void insert(unsigned char key) {
    keys[key - 'a'] = true;
  }
  bool can_open(unsigned char door) const {
    return has_key(std::tolower(door));
  }

  bool operator==(const State&) const = default;
};

template <>
struct std::hash<State> {
  auto operator()(const State& s) const {
    return my_std::ranges::fold_left(s.robots, s.keys.to_ulong(), [](auto h, Vec2 v) {
      return (h << 1) ^ std::hash<Vec2>{}(v);
    });
  }
};

auto find_available_keys(const std::size_t robot, const State& s, const Maze& m, const Graph& g) {
  std::vector<std::pair<Vec2, int>> keys;
  {
    std::unordered_set<Vec2> visited;
    for (std::deque q{std::pair{s.robots[robot], 0}}; not q.empty(); q.pop_front()) {
      auto [pos, dist]{q.front()};
      if (auto&& [_, unseen]{visited.insert(pos)}; not unseen) {
        continue;
      }
      if (char key{m.get_key(pos)}; key and not s.has_key(key)) {
        keys.emplace_back(pos, dist);
        continue;
      }
      if (char door{m.get_door(pos)}; door and not s.can_open(door)) {
        continue;
      }
      for (auto [next_pos, step] : g.at(pos)) {
        q.emplace_back(next_pos, dist + step);
      }
    }
  }
  return keys;
}

auto collect_keys(const Maze& maze, const auto& entrances) {
  const auto graphs{
      entrances | views::transform([&maze](Vec2 e) { return maze.as_graph(e); })
      | ranges::to<std::vector>()
  };

  auto dist{[steps = std::unordered_map<State, int>{}](const State& s) mutable -> auto& {
    if (not steps.contains(s)) {
      steps[s] = std::numeric_limits<int>::max();
    }
    return steps[s];
  }};

  // 1. greatest amount of keys collected
  // 2. fewest steps taken
  const auto priority{[&dist](const State& s) { return std::tuple{s.keys.count(), -dist(s)}; }};

  const auto push_max_priority_heap{[&priority](auto& q, State s) {
    q.push_back(s);
    ranges::push_heap(q, {}, priority);
  }};

  const auto pop_max_priority_heap{[&priority](auto& q) {
    ranges::pop_heap(q, {}, priority);
    State s{q.back()};
    q.pop_back();
    return s;
  }};

  int min_dist{std::numeric_limits<int>::max()};

  std::vector q{State{.robots = entrances}};
  dist(q.front()) = 0;

  while (not q.empty()) {
    State src{pop_max_priority_heap(q)};
    if (src.keys.count() == maze.keys.size()) {
      min_dist = std::min(min_dist, dist(src));
      continue;
    }
    for (auto robot{0uz}; robot < src.robots.size(); ++robot) {
      for (auto [key_pos, n_steps] : find_available_keys(robot, src, maze, graphs[robot])) {
        State dst{src};
        dst.robots[robot] = key_pos;
        dst.insert(maze.get_key(key_pos));
        if (auto dist2next{aoc::saturating_add(dist(src), n_steps)}; dist2next < dist(dst)) {
          dist(dst) = dist2next;
          push_max_priority_heap(q, dst);
        }
      }
    }
  }

  return min_dist;
}

auto parse_maze(std::string_view path) {
  Maze m;
  Vec2 entrance;
  {
    Vec2 pos;
    std::istringstream is{aoc::slurp_file(path)};
    for (std::string line; std::getline(is, line) and not line.empty(); ++pos.y()) {
      if (not m.width) {
        m.width = line.size();
      } else if (line.size() != m.width) {
        throw std::runtime_error("every row must be of same width");
      }
      pos.x() = 0;
      for (char ch : line) {
        if ('a' <= ch and ch <= 'z') {
          m.keys[pos] = ch;
        } else if ('A' <= ch and ch <= 'Z') {
          m.doors[pos] = ch;
        } else if (ch == '@') {
          entrance = pos;
        } else if (not(ch == '#' or ch == '.')) {
          throw std::runtime_error(std::format("input contains an unknown character {}", ch));
        }
        m.walls.push_back(ch == '#');
        ++pos.x();
      }
    }
    if (not is.eof()) {
      throw std::runtime_error("input contains unknown characters after the maze");
    }
  }
  return std::pair{m, entrance};
}

auto init_part2(Maze m, Vec2 entrance) {
  std::vector entrances{
      entrance + Vec2(-1, -1),
      entrance + Vec2(-1, 1),
      entrance + Vec2(1, -1),
      entrance + Vec2(1, 1),
  };
  m.walls.at(m.index(entrance)) = true;
  for (Vec2 adj : entrance.adjacent()) {
    m.walls.at(m.index(adj)) = true;
  }
  return std::pair{m, entrances};
}

int main() {
  auto [maze1, entrance1]{parse_maze("/dev/stdin")};
  auto [maze2, entrances2]{init_part2(maze1, entrance1)};

  const auto part1{collect_keys(maze1, std::vector{entrance1})};
  const auto part2{collect_keys(maze2, entrances2)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
