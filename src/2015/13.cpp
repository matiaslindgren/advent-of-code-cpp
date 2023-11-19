import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Pair {
  std::string src;
  std::string dst;
  int happiness;
};

std::istream& operator>>(std::istream& is, Pair& p) {
  const auto skip = [&is](std::string_view s) {
    using std::operator""sv;
    return ranges::all_of(views::split(s, " "sv), [&is](auto&& w) {
      std::string tmp;
      return is >> tmp && tmp == std::string_view{w};
    });
  };
  std::string sign;
  if (is >> p.src && skip("would") && is >> sign && (sign == "gain" || sign == "lose")
      && is >> p.happiness && skip("happiness units by sitting next to") && is >> p.dst
      && !p.dst.empty() && p.dst.back() == '.') {
    if (sign == "lose") {
      p.happiness = -p.happiness;
    }
    p.dst.pop_back();
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing input");
}

class Graph {
  std::vector<std::string> nodes;
  std::vector<int> weights;

 public:
  std::size_t node_count() const {
    return nodes.size();
  }

  /* //TODO(P0847R7) */
  /* auto&& happiness(this auto&& self, auto n1, auto n2) { */
  /*   return weights[n1 + n2 * node_count()]; */
  /* } */
  auto& happiness(auto n1, auto n2) {
    return weights[n1 + n2 * node_count()];
  }
  const auto& happiness(auto n1, auto n2) const {
    return weights[n1 + n2 * node_count()];
  }

  explicit Graph(const std::vector<Pair>& pairs) {
    for (const auto& p : pairs) {
      nodes.push_back(p.src);
      nodes.push_back(p.dst);
    }
    {
      ranges::sort(nodes);
      const auto duplicates = ranges::unique(nodes);
      nodes.erase(duplicates.begin(), duplicates.end());
    }

    const auto n{node_count()};
    weights.resize(n * n);
    const auto find_node_index = [this](const auto& name) {
      return ranges::distance(this->nodes.begin(), ranges::find(this->nodes, name));
    };
    for (const auto& p : pairs) {
      const auto n1{find_node_index(p.src)};
      const auto n2{find_node_index(p.dst)};
      happiness(n1, n2) = p.happiness;
    }
  }
};

constexpr auto accumulate
    = std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus<int>());

// TODO rotated view?
auto find_seating_happiness(const auto& seating, const Graph& g) {
  const auto n{g.node_count()};
  return accumulate(views::transform(views::iota(0uz, n), [&](auto i) {
    const auto& lhs{seating[i]};
    const auto& rhs{seating[(i + 1) % n]};
    return g.happiness(lhs, rhs) + g.happiness(rhs, lhs);
  }));
}

auto maximize_happiness(const auto& pairs) {
  Graph g{pairs};
  auto seating = views::iota(0uz, g.node_count()) | ranges::to<std::vector<std::size_t>>();
  int happiness{};
  do {
    happiness = std::max(happiness, find_seating_happiness(seating, g));
  } while (ranges::next_permutation(seating).found);
  return happiness;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  auto pairs = views::istream<Pair>(std::cin) | ranges::to<std::vector<Pair>>();

  const auto part1{maximize_happiness(pairs)};

  pairs.emplace_back("Me", "Anyone", 0);
  const auto part2{maximize_happiness(pairs)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
