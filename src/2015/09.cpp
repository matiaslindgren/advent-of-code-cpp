#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Edge {
  std::string src;
  std::string dst;
  int len{};
};

struct Graph {
  std::unordered_map<std::string, std::unordered_map<std::string, int>> edges;

  explicit Graph(const std::vector<Edge>& init_edges) {
    for (auto&& [src, dst, len] : init_edges) {
      edges[src][dst] = edges[dst][src] = len;
    }
  }

  [[nodiscard]]
  auto get_dist(const auto& src, const auto& dst) const {
    if (edges.contains(src) and edges.at(src).contains(dst)) {
      return edges.at(src).at(dst);
    }
    return std::numeric_limits<int>::max();
  }

  [[nodiscard]]
  auto find_all_hamiltonian_path_lengths() const {
    std::vector<int> lengths;

    struct State {
      std::unordered_set<std::string> visited;
      std::string node;
      int path_length;
    };

    for (const auto& start : edges | views::keys) {
      for (std::vector q{State{{}, start, {}}}; not q.empty();) {
        State s{q.back()};
        q.pop_back();
        if (auto [_, is_new]{s.visited.insert(s.node)}; not is_new) {
          continue;
        }
        if (s.visited.size() == edges.size()) {
          lengths.push_back(s.path_length);
          continue;
        }
        for (const auto& dst : edges | views::keys) {
          q.emplace_back(s.visited, dst, aoc::saturating_add(s.path_length, get_dist(s.node, dst)));
        }
      }
    }

    return lengths;
  }
};

std::istream& operator>>(std::istream& is, Edge& edge) {
  if (Edge e; is >> e.src >> std::ws >> skip("to"s) >> e.dst >> std::ws >> skip("="s) >> e.len) {
    edge = e;
  }
  return is;
}

int main() {
  Graph g{aoc::parse_items<Edge>("/dev/stdin")};
  const auto [part1, part2]{ranges::minmax(g.find_all_hamiltonian_path_lengths())};
  std::println("{} {}", part1, part2);
  return 0;
}
