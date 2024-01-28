import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  forest = '#',
  ground = '.',
  up = '^',
  right = '>',
  down = 'v',
  left = '<',
};

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::forest):
      case std::to_underlying(Tile::ground):
      case std::to_underlying(Tile::up):
      case std::to_underlying(Tile::right):
      case std::to_underlying(Tile::down):
      case std::to_underlying(Tile::left):
        tile = {ch};
        return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Tile");
}

struct Grid2D {
  std::vector<Tile> tiles;
  std::size_t width{};
  std::size_t height{};

  auto start_index() const {
    return width + 2;
  }
  auto end_index() const {
    return (width - 1) * height - 3;
  }

  void append_padding() {
    tiles.append_range(views::repeat(Tile::forest, width));
    ++height;
  }

  auto adjacent(const auto i) const {
    std::vector<std::size_t> adj;
    const auto t{tiles.at(i)};
    if (t == Tile::ground || t == Tile::up) {
      adj.push_back(i - width);
    }
    if (t == Tile::ground || t == Tile::right) {
      adj.push_back(i + 1);
    }
    if (t == Tile::ground || t == Tile::down) {
      adj.push_back(i + width);
    }
    if (t == Tile::ground || t == Tile::left) {
      adj.push_back(i - 1);
    }
    return adj;
  }

  auto compress_edges() const {
    std::unordered_map<std::size_t, std::unordered_map<std::size_t, int>> edges;
    for (std::vector q{start_index()}; !q.empty();) {
      const auto src{q.back()};
      q.pop_back();
      for (const auto& dst : adjacent(src)) {
        if (tiles.at(dst) == Tile::forest) {
          continue;
        }
        if (!edges[src].contains(dst)) {
          edges[src][dst] = 1;
          q.push_back(dst);
        }
      }
    }
    const auto has_2_edges{[](const auto& adj) { return adj.second.size() == 2; }};
    while (true) {
      const auto redundant_node{ranges::find_if(edges, has_2_edges)};
      if (redundant_node == edges.end()) {
        return edges;
      }
      const auto [node, adj] = *redundant_node;
      auto it{adj.begin()};
      const auto [adj1, dist1] = *it;
      const auto [adj2, dist2] = *(++it);
      edges[adj1][adj2] = edges[adj2][adj1] = dist1 + dist2;
      edges[adj1].erase(node);
      edges[adj2].erase(node);
      edges.erase(node);
    }
  }
};

std::istream& operator>>(std::istream& is, Grid2D& grid) {
  Grid2D g;
  for (std::string line; std::getline(is, line) && !line.empty(); ++g.height) {
    line.insert(line.begin(), std::to_underlying(Tile::forest));
    line.insert(line.end(), std::to_underlying(Tile::forest));
    if (!g.width) {
      g.width = line.size();
      g.append_padding();
    } else if (line.size() != g.width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    std::stringstream ls{line};
    g.tiles.append_range(views::istream<Tile>(ls));
  }
  if (g.width && g.height) {
    g.append_padding();
    grid = g;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Grid");
}

struct Graph {
  std::unordered_map<std::size_t, std::size_t> nodes;
  std::vector<std::vector<std::pair<std::size_t, int>>> edges;

  explicit Graph(const Grid2D& grid) {
    const auto all_edges{grid.compress_edges()};
    nodes = views::zip(all_edges | views::keys, views::iota(0uz, all_edges.size()))
            | ranges::to<std::unordered_map>();
    edges.resize(nodes.size());
    for (const auto& [node1, adjacent] : all_edges) {
      for (const auto& [node2, dist] : adjacent) {
        edges[nodes.at(node1)].emplace_back(nodes.at(node2), dist);
      }
    }
  }
};

template <std::size_t max_node_count>
auto find_longest_path(const Grid2D& grid) {
  Graph graph(grid);

  if (graph.edges.size() > max_node_count) {
    throw std::runtime_error("too many nodes, can't use std::bitset");
  }

  struct State {
    std::size_t current;
    int distance{};
    std::bitset<max_node_count> visited{};
  };

  const auto start{graph.nodes.at(grid.start_index())};
  const auto end{graph.nodes.at(grid.end_index())};
  int max_dist{0};

  for (std::vector q{State{.current = start}}; !q.empty();) {
    auto [curr, dist_total, visited] = q.back();
    q.pop_back();
    if (curr == end) {
      max_dist = std::max(max_dist, dist_total);
      continue;
    }
    if (visited[curr]) {
      continue;
    }
    visited[curr] = true;
    for (const auto& [next, dist] : graph.edges.at(curr)) {
      q.push_back(State{
          .current = next,
          .distance = dist_total + dist,
          .visited = visited,
      });
    }
  }

  return max_dist;
}

auto find_part1(const Grid2D& grid) {
  return find_longest_path<128>(grid);
}

auto find_part2(Grid2D grid) {
  for (auto& tile : grid.tiles) {
    switch (tile) {
      case Tile::up:
      case Tile::right:
      case Tile::down:
      case Tile::left:
        tile = Tile::ground;
      default:
        break;
    }
  }
  return find_longest_path<36>(grid);
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  Grid2D grid;
  input >> grid;

  const auto part1{find_part1(grid)};
  const auto part2{find_part2(grid)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
