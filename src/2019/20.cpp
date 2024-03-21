import std;
import aoc;
import my_std;

using std::operator""s;
using Vec2 = aoc::Vec2<int>;
using Vec3 = aoc::Vec3<int>;

struct Graph {
  std::unordered_map<Vec2, std::unordered_map<Vec2, int>> edges;
  std::unordered_map<std::string, std::vector<Vec2>> portals;
};

bool is_portal(char ch) {
  return 'A' <= ch and ch <= 'Z';
}

struct Maze {
  std::string chars{};
  std::size_t width{};

  char get(const Vec2& p) const {
    return chars.at(p.y() * width + p.x());
  }

  Graph to_graph() const {
    Graph g;

    const int h = chars.size() / width;
    const int w = width;

    for (Vec2 p(2, 2); p.y() < h - 2; ++p.y()) {
      for (p.x() = 2; p.x() < w - 2; ++p.x()) {
        if (get(p) == '.') {
          for (Vec2 step : {Vec2(0, -1), Vec2(0, 1), Vec2(-1, 0), Vec2(1, 0)}) {
            const Vec2 p1{p + step};
            const Vec2 p2{p + step + step};
            const char ch1{get(std::min(p1, p2))};
            const char ch2{get(std::max(p1, p2))};
            if (is_portal(ch1) and is_portal(ch2)) {
              g.portals[std::string{{ch1, ch2}}].push_back(p - Vec2(2, 2));
            }
          }
        }
      }
    }

    {
      std::unordered_set<Vec2> visited;
      for (Vec2 p(2, 2); p.y() < h - 2; ++p.y()) {
        for (p.x() = 2; p.x() < w - 2; ++p.x()) {
          if (get(p) == '.') {
            for (std::deque q{p}; not q.empty(); q.pop_front()) {
              Vec2 p1{q.front()};
              if (auto [_, unvisited]{visited.insert(p1)}; not unvisited) {
                continue;
              }
              for (Vec2 p2 : p1.adjacent()) {
                if (get(p2) == '.') {
                  Vec2 p1_grid{p1 - Vec2(2, 2)};
                  Vec2 p2_grid{p2 - Vec2(2, 2)};
                  g.edges[p1_grid][p2_grid] = g.edges[p2_grid][p1_grid] = 0;
                  q.emplace_back(p2);
                }
              }
            }
          }
        }
      }
    }

    std::unordered_set<Vec2> inner_portals;
    {
      std::unordered_set<Vec2> visited;
      for (std::deque q{Vec2(w / 2, h / 2)}; not q.empty(); q.pop_front()) {
        Vec2 p{q.front()};
        if (auto [_, unvisited]{visited.insert(p)}; not unvisited) {
          continue;
        }
        for (Vec2 adj : p.adjacent()) {
          if (auto ch{get(adj)}; ch == '.') {
            inner_portals.insert(adj - Vec2(2, 2));
          } else if (ch == ' ' or is_portal(ch)) {
            q.emplace_back(adj);
          }
        }
      }
    }

    for (auto jump : std::views::values(g.portals)) {
      if (jump.size() == 1) {
        continue;
      }
      if (jump.size() == 2) {
        Vec2 p1{jump.front()};
        Vec2 p2{jump.back()};
        int p1_inner{inner_portals.contains(p1)};
        int p2_inner{inner_portals.contains(p2)};
        g.edges[p1][p2] = p1_inner - p2_inner;
        g.edges[p2][p1] = p2_inner - p1_inner;
        continue;
      }
      throw std::runtime_error("portals much be of size 1 or 2");
    }

    return g;
  }
};

enum class Search {
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
  {
    std::istringstream is{aoc::slurp_file(path)};
    for (std::string line; std::getline(is, line) and not line.empty();) {
      if (not m.width) {
        m.width = line.size();
      } else if (line.size() != m.width) {
        throw std::runtime_error("every line must be of same length");
      }
      for (char ch : line) {
        if (is_portal(ch) or ch == ' ' or ch == '#' or ch == '.') {
          m.chars.push_back(ch);
        } else {
          throw std::runtime_error(std::format("maze contains an unknown char '{}'", ch));
        }
      }
    }
  }
  return m.to_graph();
}

int main() {
  const Graph graph{parse_input("/dev/stdin")};

  const auto part1{search(graph, Search::simple)};
  const auto part2{search(graph, Search::recursive)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
