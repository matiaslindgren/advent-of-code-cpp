import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

struct Graph {
  std::unordered_map<std::string, std::unordered_map<std::string, int>> edges;

  auto nodes() const {
    return edges | views::keys | ranges::to<std::vector<std::string>>();
  }

  void add_edge(const auto& n1, const auto& n2, const auto& w) {
    edges[n1][n2] = edges[n2][n1] = w;
  }

  int node_weight(const auto& n) const {
    return sum(edges.at(n) | views::values);
  }

  int cut_weight(const auto& n1, const auto& cut) const {
    int cut_w{};
    if (!cut.contains(n1) && edges.contains(n1)) {
      for (const auto& [n2, w] : edges.at(n1)) {
        if (cut.contains(n2)) {
          cut_w += w;
        }
      }
    }
    return cut_w;
  }
};

std::istream& operator>>(std::istream& is, Graph& graph) {
  Graph g;
  for (std::string line; std::getline(is, line) && !line.empty();) {
    std::stringstream ls{line};
    if (std::string lhs; ls >> lhs && lhs.ends_with(":"s)) {
      lhs.pop_back();
      for (auto rhs : views::istream<std::string>(ls)) {
        g.add_edge(lhs, rhs, 1);
      }
    } else {
      is.setstate(std::ios_base::failbit);
      break;
    }
  }
  if (is.eof()) {
    graph = g;
    return is;
  }
  throw std::runtime_error("failed parsing Graph");
}

auto find_part1(Graph graph) {
  // Stoer–Wagner algorithm
  // https://en.wikipedia.org/wiki/Stoer%E2%80%93Wagner_algorithm
  auto best_weight{std::numeric_limits<int>::max()};
  auto best_cut{""s};
  const auto nodes{graph.nodes()};
  auto cuts{
      views::zip(
          nodes,
          views::transform(
              nodes,
              [](const auto& n) -> std::unordered_set<std::string> { return {n}; }
          )
      )
      | ranges::to<std::unordered_map>()
  };

  for (auto phase{0uz}; phase < nodes.size() - 1; ++phase) {
    auto s{""s};
    auto t{""s};
    {
      auto q = graph.nodes();
      s = t = q.back();
      q.pop_back();

      std::unordered_set A = {s};
      const auto get_cut_weight{[&graph, &A](const auto& n) { return graph.cut_weight(n, A); }};

      while (!q.empty()) {
        s = t;
        const auto next{ranges::max_element(q, {}, get_cut_weight)};
        t = *next;
        q.erase(next);
        A.insert(t);
      }
    }
    if (const auto new_weight{graph.node_weight(t)}; new_weight < best_weight) {
      best_weight = new_weight;
      best_cut = t;
    }
    for (const auto& [adj_t, adj_t_w] : graph.edges[t]) {
      if (adj_t != s) {
        graph.edges[s][adj_t] += adj_t_w;
        graph.edges[adj_t][s] += adj_t_w;
      }
    }
    for (const auto& n : graph.nodes()) {
      graph.edges[n].erase(t);
      if (graph.edges[n].empty()) {
        graph.edges.erase(n);
      }
    }
    graph.edges.erase(t);
    cuts[s].insert_range(cuts[t]);
    /* std::print("phase {} s {} t {} bw {} bc {}\n", phase, s, t, best_weight, best_cut); */
  }

  const auto bc{cuts[best_cut].size()};
  return bc * (nodes.size() - bc);
}

int main() {
  aoc::init_io();

  Graph graph;
  std::cin >> graph;

  const auto part1{find_part1(graph)};

  std::print("{}\n", part1);

  return 0;
}
