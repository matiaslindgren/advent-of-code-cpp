import std;

namespace ranges = std::ranges;
namespace views = std::views;

template <typename UInt>
  requires std::unsigned_integral<UInt>
constexpr auto saturating_add(UInt a, UInt b) {
  constexpr auto limit{std::numeric_limits<UInt>::max()};
  return (a > limit - b) ? limit : a + b;
}

struct Edge {
  std::string src;
  std::string dst;
  std::size_t dist;
};

std::istream& operator>>(std::istream& is, Edge& edge) {
  std::string src, dst, tmp;
  std::size_t dist;
  if (is >> src && is >> tmp && tmp == "to" && is >> dst && is >> tmp && tmp == "=" && is >> dist) {
    edge = {src, dst, dist};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Edge");
}

class Graph {
  std::size_t node_count_;
  std::vector<std::size_t> distances;

 public:
  constexpr explicit Graph(const std::vector<Edge>& edges) {
    std::vector<std::string> nodes;
    for (const auto& edge : edges) {
      nodes.push_back(edge.src);
      nodes.push_back(edge.dst);
    }
    {
      ranges::sort(nodes);
      const auto duplicates{ranges::unique(nodes)};
      nodes.erase(duplicates.begin(), duplicates.end());
    }
    node_count_ = nodes.size();

    distances.resize(node_count() * node_count(), max_distance());
    for (auto n : views::iota(0uz, node_count())) {
      distance(n, n) = 0;
    }

    const auto find_node_index{[&nodes](const auto& name) {
      return ranges::distance(nodes.begin(), ranges::find(nodes, name));
    }};
    for (const auto& e : edges) {
      const auto n1{find_node_index(e.src)};
      const auto n2{find_node_index(e.dst)};
      distance(n1, n2) = distance(n2, n1) = e.dist;
    }
  }

  constexpr std::size_t node_count() const {
    return node_count_;
  }

  constexpr std::size_t max_distance() const {
    return std::numeric_limits<std::size_t>::max();
  }

  /* //TODO(P0847R7) */
  /* auto&& distance(this auto&& self, auto n1, auto n2) { */
  /*   return distances[n1 + n2 * node_count()]; */
  /* } */
  std::size_t& distance(auto n1, auto n2) {
    return distances[n1 + n2 * node_count()];
  }
  const std::size_t& distance(auto n1, auto n2) const {
    return distances[n1 + n2 * node_count()];
  }

  template <std::size_t max_node_count>
  constexpr auto find_all_hamiltonian_path_lengths() const {
    if (max_node_count < node_count()) {
      throw std::runtime_error("too many nodes");
    }
    std::vector<std::size_t> lengths;
    struct State {
      std::bitset<max_node_count> visited;
      std::size_t node;
      std::size_t path_length;
    };
    for (auto start : views::iota(0uz, node_count())) {
      std::vector<State> q = {{{}, start, {}}};
      while (!q.empty()) {
        auto current{q.back()};
        q.pop_back();
        if (current.visited[current.node]) {
          continue;
        }
        current.visited[current.node] = true;
        if (current.visited.count() == node_count()) {
          lengths.push_back(current.path_length);
          continue;
        }
        for (auto node : views::iota(0uz, node_count())) {
          const auto edge_length{distance(current.node, node)};
          q.emplace_back(current.visited, node, saturating_add(current.path_length, edge_length));
        }
      }
    }
    return lengths;
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  constexpr auto max_node_count{8};

  const auto edges{views::istream<Edge>(std::cin) | ranges::to<std::vector<Edge>>()};

  Graph g{edges};
  const auto hamiltonian_path_lengths{g.find_all_hamiltonian_path_lengths<max_node_count>()};
  const auto [shortest_path, longest_path] = ranges::minmax_element(hamiltonian_path_lengths);

  const auto part1{*shortest_path};
  const auto part2{*longest_path};
  std::print("{} {}\n", part1, part2);

  return 0;
}
