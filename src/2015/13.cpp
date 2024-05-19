#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Pair {
  std::string src;
  std::string dst;
  int happiness;
};

std::istream& operator>>(std::istream& is, Pair& p) {
  using std::operator""s;
  using aoc::skip;
  std::string sign;
  if (is >> p.src >> skip(" would"s) >> sign and (sign == "gain" or sign == "lose")
      and is >> p.happiness >> skip(" happiness units by sitting next to"s) >> p.dst
      and not p.dst.empty() and p.dst.back() == '.') {
    if (sign == "lose") {
      p.happiness = -p.happiness;
    }
    p.dst.pop_back();
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Pair");
}

class Graph {
  std::vector<std::string> nodes;
  std::vector<int> weights;

 public:
  std::size_t node_count() const {
    return nodes.size();
  }

  auto&& happiness(this auto&& self, auto n1, auto n2) {
    return self.weights[n1 + n2 * self.node_count()];
  }

  explicit Graph(const std::vector<Pair>& pairs) {
    for (const auto& p : pairs) {
      nodes.push_back(p.src);
      nodes.push_back(p.dst);
    }
    {
      ranges::sort(nodes);
      const auto duplicates{ranges::unique(nodes)};
      nodes.erase(duplicates.begin(), duplicates.end());
    }

    const auto n{node_count()};
    weights.resize(n * n);
    const auto find_node_index{[this](const auto& name) {
      return ranges::distance(this->nodes.begin(), ranges::find(this->nodes, name));
    }};
    for (const auto& p : pairs) {
      const auto n1{find_node_index(p.src)};
      const auto n2{find_node_index(p.dst)};
      happiness(n1, n2) = p.happiness;
    }
  }
};

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

// TODO rotated view?
auto find_seating_happiness(const auto& seating, const Graph& g) {
  const auto n{g.node_count()};
  return sum(views::transform(views::iota(0uz, n), [&](auto i) {
    const auto& lhs{seating[i]};
    const auto& rhs{seating[(i + 1) % n]};
    return g.happiness(lhs, rhs) + g.happiness(rhs, lhs);
  }));
}

auto maximize_happiness(const auto& pairs) {
  Graph g{pairs};
  auto seating = views::iota(0uz, g.node_count()) | ranges::to<std::vector>();
  int happiness{};
  do {
    happiness = std::max(happiness, find_seating_happiness(seating, g));
  } while (ranges::next_permutation(seating).found);
  return happiness;
}

int main() {
  auto pairs{aoc::parse_items<Pair>("/dev/stdin")};

  const auto part1{maximize_happiness(pairs)};

  pairs.emplace_back("Me", "Anyone", 0);
  const auto part2{maximize_happiness(pairs)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
