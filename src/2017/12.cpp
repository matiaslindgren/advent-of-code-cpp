#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Graph {
  std::unordered_map<int, std::vector<int>> edges;

  auto bfs(const auto& begin) const {
    std::unordered_set<int> visited;
    for (std::deque q{begin}; not q.empty(); q.pop_front()) {
      if (const auto& src{q.front()}; not visited.contains(src)) {
        visited.insert(src);
        q.append_range(edges.at(src));
      }
    }
    return visited;
  }
};

void dedup(auto& v) {
  ranges::sort(v);
  const auto rm{ranges::unique(v)};
  v.erase(rm.begin(), rm.end());
}

std::istream& operator>>(std::istream& is, Graph& graph) {
  Graph g;
  for (std::string line; std::getline(is, line);) {
    ranges::replace(line, ',', ' ');
    std::stringstream ls{line};
    if (int src; ls >> src >> std::ws >> skip("<->"s)) {
      const auto adjacent{views::istream<int>(ls) | ranges::to<std::vector>()};
      if (adjacent.empty()) {
        is.setstate(std::ios_base::failbit);
      } else {
        for (const int dst : adjacent) {
          g.edges[src].push_back(dst);
          g.edges[dst].push_back(src);
        }
      }
    }
  }
  if (is or is.eof()) {
    for (auto& adj : g.edges | views::values) {
      dedup(adj);
    }
    graph = g;
    return is;
  }
  throw std::runtime_error("failed parsing Graph");
}

auto find_group_sizes(const Graph& g) {
  auto visited{g.bfs(0)};
  const auto group0_size{visited.size()};

  int group_count{1};
  for (const auto& node : views::keys(g.edges)) {
    if (not visited.contains(node)) {
      visited.insert_range(g.bfs(node));
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
