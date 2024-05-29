#include "aoc.hpp"
#include "std.hpp"

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Key = std::string;

struct Edge {
  Key src;
  Key dst;
};

auto find_part1(const auto& parents) {
  int n_orbits{};
  for (Key n : parents | views::keys) {
    for (; parents.contains(n); n = parents.at(n)) {
      n_orbits += 1;
    }
  }
  return n_orbits;
}

auto find_part2(const auto& parents) {
  Key lca{};
  {
    std::unordered_set<Key> seen;
    for (Key lhs{"YOU"s}, rhs{"SAN"s}; lca.empty();) {
      if (auto&& [_, is_new]{seen.insert(lhs)}; not is_new) {
        lca = lhs;
      } else if (auto&& [_, is_new]{seen.insert(rhs)}; not is_new) {
        lca = rhs;
      }
      lhs = parents.at(lhs);
      rhs = parents.at(rhs);
    }
  }
  int n_transfers{};
  for (Key lhs{"YOU"s}; lhs != lca; lhs = parents.at(lhs)) {
    n_transfers += 1;
  }
  for (Key rhs{"SAN"s}; rhs != lca; rhs = parents.at(rhs)) {
    n_transfers += 1;
  }
  return n_transfers - 2;
}

std::istream& operator>>(std::istream& is, Edge& edge) {
  if (std::string line; std::getline(is, line) and line.size() > 2) {
    const auto delim{ranges::find(line, ')')};
    if (line.begin() < delim and delim + 1 < line.end()) {
      edge = {
          .src = {line.begin(), delim},
          .dst = {delim + 1, line.end()},
      };
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Edge");
}

int main() {
  const auto parents{
      aoc::parse_items<Edge>("/dev/stdin")
      | views::transform([](const Edge& e) { return std::pair{e.dst, e.src}; })
      | ranges::to<std::unordered_map>()
  };
  const auto part1{find_part1(parents)};
  const auto part2{find_part2(parents)};

  std::println("{} {}", part1, part2);

  return 0;
}
