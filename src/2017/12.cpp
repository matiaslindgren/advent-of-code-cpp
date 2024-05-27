#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Graph {
  std::unordered_map<int, std::vector<int>> edges;

  [[nodiscard]]
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

Graph parse_graph(std::string_view path) {
  Graph g;
  for (std::string line : aoc::slurp_lines(path)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    if (int src{}; ls >> src >> std::ws >> skip("<->"s)) {
      const auto adjacent{views::istream<int>(ls) | ranges::to<std::vector>()};
      if (adjacent.empty()) {
        throw std::runtime_error(std::format("line does not contain adjacencies: '{}'", line));
      }
      for (int dst : adjacent) {
        g.edges[src].push_back(dst);
        g.edges[dst].push_back(src);
      }
    }
  }
  for (auto& adj : g.edges | views::values) {
    dedup(adj);
  }
  return g;
}

int main() {
  Graph g{parse_graph("/dev/stdin")};
  const auto [part1, part2]{find_group_sizes(g)};
  std::println("{} {}", part1, part2);
  return 0;
}
