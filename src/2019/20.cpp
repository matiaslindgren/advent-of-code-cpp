#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using std::operator""s;
using Vec2 = ndvec::vec2<int>;
using Vec3 = ndvec::vec3<int>;

struct Graph {
  std::unordered_map<Vec2, std::unordered_map<Vec2, int>> edges;
  std::unordered_map<std::string, std::vector<Vec2>> portals;
};

bool is_portal(char ch) {
  return 'A' <= ch and ch <= 'Z';
}

struct Maze {
  std::string chars;
  int width{};
  int height{};

  [[nodiscard]]
  char get(const Vec2& p) const {
    return chars.at(p.y() * width + p.x());
  }

  void into_portals(auto& portals) const {
    for (Vec2 p(2, 2); p.y() < height - 2; ++p.y()) {
      for (p.x() = 2; p.x() < width - 2; ++p.x()) {
        if (get(p) == '.') {
          for (Vec2 step : {Vec2(0, -1), Vec2(0, 1), Vec2(-1, 0), Vec2(1, 0)}) {
            const Vec2 p1{p + step};
            const Vec2 p2{p + step + step};
            const char ch1{get(std::min(p1, p2))};
            const char ch2{get(std::max(p1, p2))};
            if (is_portal(ch1) and is_portal(ch2)) {
              portals[std::string{{ch1, ch2}}].push_back(p - Vec2(2, 2));
            }
          }
        }
      }
    }
  }

  void into_edges(auto& edges) const {
    std::unordered_set<Vec2> visited;
    for (Vec2 p(2, 2); p.y() < height - 2; ++p.y()) {
      for (p.x() = 2; p.x() < width - 2; ++p.x()) {
        if (get(p) != '.') {
          continue;
        }
        for (std::deque q{p}; not q.empty(); q.pop_front()) {
          Vec2 p1{q.front()};
          if (auto [_, unvisited]{visited.insert(p1)}; not unvisited) {
            continue;
          }
          for (Vec2 p2 : p1.adjacent()) {
            if (get(p2) == '.') {
              Vec2 p1_grid{p1 - Vec2(2, 2)};
              Vec2 p2_grid{p2 - Vec2(2, 2)};
              edges[p1_grid][p2_grid] = edges[p2_grid][p1_grid] = 0;
              q.emplace_back(p2);
            }
          }
        }
      }
    }
  }

  void into_inner_portals(auto& portals) const {
    std::unordered_set<Vec2> visited;
    for (std::deque q{Vec2(width / 2, height / 2)}; not q.empty(); q.pop_front()) {
      Vec2 p{q.front()};
      if (auto [_, unvisited]{visited.insert(p)}; unvisited) {
        for (Vec2 adj : p.adjacent()) {
          if (auto ch{get(adj)}; ch == '.') {
            portals.insert(adj - Vec2(2, 2));
          } else if (ch == ' ' or is_portal(ch)) {
            q.emplace_back(adj);
          }
        }
      }
    }
  }

  [[nodiscard]]
  Graph to_graph() const {
    Graph g;

    into_portals(g.portals);
    into_edges(g.edges);

    std::unordered_set<Vec2> inner_portals;
    into_inner_portals(inner_portals);

    for (auto jump : std::views::values(g.portals)) {
      if (jump.size() == 1) {
        continue;
      }
      if (jump.size() == 2) {
        Vec2 p1{jump.front()};
        Vec2 p2{jump.back()};
        auto p1_inner{int{inner_portals.contains(p1)}};
        auto p2_inner{int{inner_portals.contains(p2)}};
        g.edges[p1][p2] = p1_inner - p2_inner;
        g.edges[p2][p1] = p2_inner - p1_inner;
        continue;
      }
      throw std::runtime_error("portals much be of size 1 or 2");
    }

    return g;
  }
};

enum class Search : unsigned char {
  simple,
  recursive,
};

auto search(const Graph& g, Search type) {
  Vec2 aa{g.portals.at("AA"s).at(0)};
  Vec2 zz{g.portals.at("ZZ"s).at(0)};

  Vec3 begin{aa.x(), aa.y(), 0};
  Vec3 target{zz.x(), zz.y(), 0};

  std::unordered_set<Vec3> visited;

  for (std::deque q{std::pair{begin, 0}}; not q.empty(); q.pop_front()) {
    auto [src, step]{q.front()};
    if (src.z() < 0) {
      continue;
    }
    if (src == target) {
      return step;
    }
    if (auto [_, unvisited]{visited.insert(src)}; not unvisited) {
      continue;
    }
    for (auto [dst, level] : g.edges.at(Vec2(src.x(), src.y()))) {
      if (type == Search::simple) {
        level = 0;
      }
      q.emplace_back(Vec3{dst.x(), dst.y(), src.z() + level}, step + 1);
    }
  }

  throw std::runtime_error(std::format("could not find path from {} to {}", begin, target));
}

Graph parse_input(std::string_view path) {
  Maze m;
  for (const std::string& line : aoc::slurp_lines(path)) {
    int width{};
    for (char ch : line) {
      if (is_portal(ch) or ch == ' ' or ch == '#' or ch == '.') {
        m.chars.push_back(ch);
      } else {
        throw std::runtime_error(std::format("maze contains an unknown char '{}'", ch));
      }
      width += 1;
    }
    if (m.width == 0) {
      m.width = width;
    } else if (width != m.width) {
      throw std::runtime_error("every line must be of same length");
    }
    m.height += 1;
  }
  return m.to_graph();
}

int main() {
  const Graph graph{parse_input("/dev/stdin")};

  const auto part1{search(graph, Search::simple)};
  const auto part2{search(graph, Search::recursive)};

  std::println("{} {}", part1, part2);

  return 0;
}
