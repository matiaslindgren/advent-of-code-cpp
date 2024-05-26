#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  forest = '#',
  ground = '.',
  up = '^',
  right = '>',
  down = 'v',
  left = '<',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  int width{};
  int height{};

  [[nodiscard]]
  Tile get(const Vec2& p) const {
    return tiles.contains(p) ? tiles.at(p) : Tile::forest;
  }

  [[nodiscard]]
  auto adjacent(const Vec2& p) const {
    std::vector<Vec2> adj;
    Tile t{get(p)};
    if (t == Tile::ground or t == Tile::up) {
      adj.push_back(p - Vec2(0, 1));
    }
    if (t == Tile::ground or t == Tile::right) {
      adj.push_back(p + Vec2(1, 0));
    }
    if (t == Tile::ground or t == Tile::down) {
      adj.push_back(p + Vec2(0, 1));
    }
    if (t == Tile::ground or t == Tile::left) {
      adj.push_back(p - Vec2(1, 0));
    }
    return adj;
  }

  [[nodiscard]]
  auto compress_edges() const {
    std::unordered_map<Vec2, std::unordered_map<Vec2, int>> edges;
    for (std::vector q{Vec2(1, 0)}; not q.empty();) {
      Vec2 src{q.back()};
      q.pop_back();
      for (Vec2 dst : adjacent(src)) {
        if (get(dst) != Tile::forest) {
          if (not edges[src].contains(dst)) {
            edges[src][dst] = 1;
            q.push_back(dst);
          }
        }
      }
    }
    const auto has_2_edges{[](const auto& adj) { return adj.second.size() == 2; }};
    while (true) {
      const auto redundant_node{ranges::find_if(edges, has_2_edges)};
      if (redundant_node == edges.end()) {
        break;
      }
      const auto [node, adj]{*redundant_node};
      auto it{adj.begin()};
      const auto [adj1, dist1]{*it};
      const auto [adj2, dist2]{*(++it)};
      edges[adj1][adj2] = edges[adj2][adj1] = dist1 + dist2;
      edges[adj1].erase(node);
      edges[adj2].erase(node);
      edges.erase(node);
    }
    return edges;
  }
};

struct Graph {
  using Node = std::size_t;
  std::unordered_map<Vec2, Node> node_ids;
  std::vector<std::vector<std::pair<Node, int>>> edges;

  explicit Graph(const Grid& grid) {
    const auto all_edges{grid.compress_edges()};
    node_ids = views::zip(all_edges | views::keys, views::iota(Node{0}, all_edges.size()))
               | ranges::to<std::unordered_map>();
    edges.resize(node_ids.size());
    for (const auto& [node1, adjacent] : all_edges) {
      for (const auto& [node2, dist] : adjacent) {
        edges[node_ids.at(node1)].emplace_back(node_ids.at(node2), dist);
      }
    }
  }
};

template <unsigned max_node_count>
auto find_longest_path(const Grid& grid) {
  Graph graph(grid);

  if (graph.edges.size() > max_node_count) {
    throw std::runtime_error("too many nodes, can't use std::bitset");
  }

  struct State {
    Graph::Node current{};
    int distance{};
    std::bitset<max_node_count> visited{};
  };

  const Graph::Node start{graph.node_ids.at(Vec2(1, 0))};
  const Graph::Node end{graph.node_ids.at(Vec2(grid.width - 2, grid.height - 1))};
  int max_dist{};

  for (std::vector q{State{.current = start}}; not q.empty();) {
    auto [curr, dist_total, visited]{q.back()};
    q.pop_back();
    if (curr == end) {
      max_dist = std::max(max_dist, dist_total);
      continue;
    }
    if (not visited.test(curr)) {
      visited.set(curr);
      for (auto&& [next, dist] : graph.edges.at(curr)) {
        q.push_back(State{
            .current = next,
            .distance = dist_total + dist,
            .visited = visited,
        });
      }
    }
  }

  return max_dist;
}

auto find_part1(const Grid& grid) {
  return find_longest_path<128>(grid);
}

auto find_part2(Grid grid) {
  for (Tile& t : grid.tiles | views::values) {
    switch (t) {
      case Tile::up:
      case Tile::right:
      case Tile::down:
      case Tile::left:
        t = Tile::ground;
      default:
        break;
    }
  }
  return find_longest_path<36>(grid);
}

auto parse_grid(std::string_view path) {
  Grid g{};
  Vec2 p;
  for (const std::string& line : aoc::slurp_lines(path)) {
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::forest):
        case std::to_underlying(Tile::ground):
        case std::to_underlying(Tile::up):
        case std::to_underlying(Tile::right):
        case std::to_underlying(Tile::down):
        case std::to_underlying(Tile::left):
          g.tiles[p] = {ch};
          break;
        default:
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
    if (g.width == 0) {
      g.width = p.x();
    } else if (g.width != p.x()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.y() += 1;
  }
  g.height = p.y();
  return g;
}

int main() {
  const Grid g{parse_grid("/dev/stdin")};

  const auto part1{find_part1(g)};
  const auto part2{find_part2(g)};

  std::println("{} {}", part1, part2);

  return 0;
}
