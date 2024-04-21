#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Graph {
  using Node = std::size_t;
  using Weight = int;
  using EdgeMap = std::unordered_map<std::string, std::unordered_map<std::string, Weight>>;

  std::size_t size{};
  std::vector<std::vector<Weight>> edges{};
  std::vector<std::unordered_set<Node>> adjacent{};

  explicit Graph() = default;

  explicit Graph(const EdgeMap& edge_map) {
    std::unordered_map<std::string, Node> node_ids;
    for (const auto& adjacent : edge_map | views::values) {
      for (const auto& node : adjacent | views::keys) {
        if (not node_ids.contains(node)) {
          node_ids[node] = node_ids.size();
        }
      }
    }
    size = node_ids.size();
    edges.resize(size, std::vector<Weight>(size, 0));
    adjacent.resize(size);
    for (const auto& [node1, adjacent] : edge_map) {
      for (const auto& [node2, weight] : adjacent) {
        const auto n1{node_ids.at(node1)};
        const auto n2{node_ids.at(node2)};
        insert_edge(n1, n2, weight);
        insert_edge(n2, n1, weight);
      }
    }
  }

  void insert_edge(const auto& n1, const auto& n2, const auto& weight) {
    edges[n1][n2] = weight;
    adjacent[n1].insert(n2);
  }

  void erase_edge(const auto& n1, const auto& n2) {
    edges[n1][n2] = 0;
    adjacent[n1].erase(n2);
  }

  auto nodes() const {
    return views::iota(Node{}, size) | ranges::to<std::vector>();
  }
};

std::istream& operator>>(std::istream& is, Graph& graph) {
  Graph::EdgeMap edges;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    std::stringstream ls{line};
    if (std::string lhs; ls >> lhs and lhs.ends_with(":"s)) {
      lhs.pop_back();
      for (auto rhs : views::istream<std::string>(ls)) {
        edges[lhs][rhs] = edges[rhs][lhs] = Graph::Weight{1};
      }
    } else {
      is.setstate(std::ios_base::failbit);
      break;
    }
  }
  if (is.eof() and not edges.empty()) {
    graph = Graph{edges};
    return is;
  }
  throw std::runtime_error("failed parsing Graph");
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto find_part1(Graph graph) {
  // Stoer–Wagner algorithm
  // https://en.wikipedia.org/wiki/Stoer%E2%80%93Wagner_algorithm
  // (2024-01-06)

  auto nodes{graph.nodes()};
  auto cuts{
      views::transform(nodes, [](const auto& n) -> std::unordered_set<Graph::Node> { return {n}; })
      | ranges::to<std::vector>()
  };
  const auto graph_size{nodes.size()};

  auto best_weight{std::numeric_limits<Graph::Weight>::max()};
  auto best_cut{0uz};

  for (auto phase{0uz}; phase < graph_size - 1; ++phase) {
    auto s{nodes.front()};
    std::vector<Graph::Node> q;
    std::unordered_map<Graph::Node, Graph::Weight> cut_weights;

    const auto get_cut_weight{[&](const auto& n) { return cut_weights[n]; }};

    const auto push_max_cut_weight_heap{[&](auto& q, const auto& n, const auto& w) {
      cut_weights[n] = w;
      q.push_back(n);
      ranges::push_heap(q, ranges::less{}, get_cut_weight);
    }};

    const auto pop_max_cut_weight_heap{[&](auto& q) {
      ranges::pop_heap(q, ranges::less{}, get_cut_weight);
      auto n{q.back()};
      q.pop_back();
      return n;
    }};

    for (const auto& n : graph.adjacent.at(s)) {
      push_max_cut_weight_heap(q, n, graph.edges[s][n]);
    }

    std::unordered_set A{s};
    for (auto i{phase + 1}; i < graph_size - 1; ++i) {
      s = pop_max_cut_weight_heap(q);
      A.insert(s);
      cut_weights.erase(s);
      for (const auto& n : graph.adjacent.at(s)) {
        if (not A.contains(n)) {
          if (cut_weights.contains(n)) {
            cut_weights[n] += graph.edges[s][n];
          } else {
            push_max_cut_weight_heap(q, n, graph.edges[s][n]);
          }
        }
      }
    }

    const auto t{pop_max_cut_weight_heap(q)};

    if (const auto new_weight{sum(graph.edges.at(t))}; new_weight < best_weight) {
      best_weight = new_weight;
      best_cut = cuts[t].size();
    }
    cuts[s].insert_range(cuts[t]);

    auto w_t{graph.edges[t]};
    w_t[s] = 0;
    for (const auto& n : nodes) {
      graph.edges[s][n] += w_t[n];
      graph.edges[n][s] += w_t[n];
    }
    for (const auto& n : nodes) {
      graph.erase_edge(n, t);
      graph.erase_edge(t, n);
    }
    nodes.erase(ranges::find(nodes, t));
  }

  return best_cut * (graph_size - best_cut);
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  Graph graph;
  input >> graph;

  const auto part1{find_part1(graph)};

  std::print("{}\n", part1);

  return 0;
}
