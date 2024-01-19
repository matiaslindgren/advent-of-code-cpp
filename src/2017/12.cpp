import std;
import aoc;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Graph {
  std::unordered_map<int, std::unordered_set<int>> edges;

  auto bfs(const auto& begin) const {
    std::vector<bool> visited(edges.size(), false);
    for (std::deque q = {begin}; !q.empty(); q.pop_front()) {
      if (const auto& src{q.front()}; !visited[src]) {
        visited[src] = true;
        q.append_range(edges.at(src));
      }
    }
    return visited;
  }
};

std::istream& operator>>(std::istream& is, Graph& graph) {
  Graph g;
  for (std::string line; std::getline(is, line);) {
    ranges::replace(line, ',', ' ');
    std::stringstream ls{line};
    if (int src; ls >> src >> std::ws && skip(ls, "<->"s)) {
      const auto adjacent{views::istream<int>(ls) | ranges::to<std::vector>()};
      if (adjacent.empty()) {
        is.setstate(std::ios_base::failbit);
      } else {
        for (const int dst : adjacent) {
          g.edges[src].insert(dst);
          g.edges[dst].insert(src);
        }
      }
    }
  }
  if (is || is.eof()) {
    graph = g;
    return is;
  }
  throw std::runtime_error("failed parsing Graph");
}

auto find_group_sizes(const Graph& g) {
  auto visited{g.bfs(0)};
  const auto group0_size{ranges::count_if(visited, std::identity{})};

  int group_count{1};
  for (const auto& node : views::keys(g.edges)) {
    if (!visited.at(node)) {
      ranges::transform(g.bfs(node), visited, visited.begin(), std::logical_or{});
      group_count += 1;
    }
  }

  return std::pair{group0_size, group_count};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  Graph graph;
  input >> graph;

  const auto [part1, part2] = find_group_sizes(graph);
  std::print("{} {}\n", part1, part2);

  return 0;
}
