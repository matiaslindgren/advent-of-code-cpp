import std;

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
  std::string str;
  if (is >> str) {
    edge.src = str;
    if (is >> str && str == "to" && is >> str) {
      edge.dst = str;
      std::size_t dist;
      if (is >> str && str == "=" && is >> dist) {
        edge.dist = dist;
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing input");
}

template <typename NodeName = std::string,
          typename Distance = std::size_t,
          typename NodeId = std::size_t>
class Graph {
  Distance node_count_;
  std::vector<Distance> distances;

 public:
  explicit Graph(const std::vector<Edge>& edges) {
    std::vector<NodeName> nodes;
    for (const auto& edge : edges) {
      nodes.push_back(edge.src);
      nodes.push_back(edge.dst);
    }
    {
      std::ranges::sort(nodes);
      const auto duplicates = std::ranges::unique(nodes);
      nodes.erase(duplicates.begin(), duplicates.end());
    }
    node_count_ = nodes.size();

    distances.resize(node_count() * node_count(), max_distance());
    for (NodeId n{}; n < node_count(); ++n) {
      distance(n, n) = 0;
    }

    const auto find_node_index = [&nodes](const auto& name) {
      return std::ranges::distance(nodes.begin(),
                                   std::ranges::find(nodes, name));
    };
    for (const auto& e : edges) {
      const auto n1{find_node_index(e.src)};
      const auto n2{find_node_index(e.dst)};
      distance(n1, n2) = distance(n2, n1) = e.dist;
    }
  }

  auto node_count() const {
    return node_count_;
  }

  constexpr auto max_distance() const {
    return std::numeric_limits<Distance>::max();
  }

  /* //TODO(P0847R7) */
  /* auto&& distance(this auto&& self, auto n1, auto n2) { */
  /*   return distances[n1 + n2 * node_count()]; */
  /* } */
  auto& distance(auto n1, auto n2) {
    return distances[n1 + n2 * node_count()];
  }
  const auto& distance(auto n1, auto n2) const {
    return distances[n1 + n2 * node_count()];
  }

  template <Distance max_node_count>
  auto find_all_hamiltonian_path_lengths() const {
    if (max_node_count < node_count()) {
      throw std::runtime_error("too many nodes");
    }
    std::vector<Distance> lengths;
    struct State {
      std::bitset<max_node_count> visited;
      NodeId node;
      Distance path_length;
    };
    for (NodeId start{}; start < node_count(); ++start) {
      std::vector<State> q = {
          {{}, start, {}}
      };
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
        for (NodeId node{}; node < node_count(); ++node) {
          const auto edge_length{distance(current.node, node)};
          q.emplace_back(current.visited,
                         node,
                         saturating_add(current.path_length, edge_length));
        }
      }
    }
    return lengths;
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  constexpr auto max_node_count{8};

  const auto edges = std::views::istream<Edge>(std::cin) |
                     std::ranges::to<std::vector<Edge>>();

  Graph g{edges};
  const auto hamiltonian_path_lengths{
      g.find_all_hamiltonian_path_lengths<max_node_count>()};
  const auto [shortest_path, longest_path] =
      std::ranges::minmax_element(hamiltonian_path_lengths);

  const auto part1{*shortest_path};
  const auto part2{*longest_path};
  std::print("{} {}\n", part1, part2);

  return 0;
}
