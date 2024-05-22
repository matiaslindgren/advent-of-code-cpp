#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Pair {
  std::string src;
  std::string dst;
  int happiness{};
};

std::istream& operator>>(std::istream& is, Pair& p) {
  using std::operator""s;
  using aoc::skip;
  std::string sign;
  if (is >> p.src >> std::ws >> skip("would"s) >> sign and (sign == "gain" or sign == "lose")
      and is >> p.happiness >> std::ws >> skip("happiness units by sitting next to"s) >> p.dst
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

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

class Graph {
  std::map<std::string, std::unordered_map<std::string, int>> edges;

 public:
  explicit Graph(const std::vector<Pair>& pairs) {
    for (auto&& [src, dst, happiness] : pairs) {
      edges[src][dst] = happiness;
    }
  }

  [[nodiscard]]
  auto nodes() const {
    return edges | views::keys | ranges::to<std::vector>();
  }

  [[nodiscard]]
  int happiness(const auto& n1, const auto& n2) const {
    if (edges.contains(n1) and edges.at(n1).contains(n2)) {
      return edges.at(n1).at(n2);
    }
    return 0;
  }

  [[nodiscard]]
  auto find_seating_happiness(const auto& seating) const {
    return sum(views::transform(views::iota(0UZ, seating.size()), [&](auto i) {
      const auto& lhs{seating.at(i)};
      const auto& rhs{seating.at((i + 1) % seating.size())};
      return happiness(lhs, rhs) + happiness(rhs, lhs);
    }));
  }
};

auto maximize_happiness(const auto& pairs) {
  Graph g{pairs};
  auto seating{g.nodes()};
  int happiness{};
  do {
    happiness = std::max(happiness, g.find_seating_happiness(seating));
  } while (ranges::next_permutation(seating).found);
  return happiness;
}

int main() {
  auto pairs{aoc::parse_items<Pair>("/dev/stdin")};

  const auto part1{maximize_happiness(pairs)};
  pairs.emplace_back("Me", "Anyone", 0);
  const auto part2{maximize_happiness(pairs)};

  std::println("{} {}", part1, part2);

  return 0;
}
