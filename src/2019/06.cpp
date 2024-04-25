#include "aoc.hpp"
#include "std.hpp"

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Key = std::string;

struct Edge {
  Key src, dst;
};

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

auto find_part1(const auto& parent) {
  int n_orbits{};
  for (Key n : parent | views::keys) {
    for (; parent.contains(n); n = parent.at(n)) {
      n_orbits += 1;
    }
  }
  return n_orbits;
}

auto find_part2(const auto& parent) {
  Key lca{};
  {
    std::unordered_set<Key> seen;
    for (Key lhs{"YOU"s}, rhs{"SAN"s}; lca.empty();) {
      if (auto&& [_, is_new]{seen.insert(lhs)}; not is_new) {
        lca = lhs;
      } else if (auto&& [_, is_new]{seen.insert(rhs)}; not is_new) {
        lca = rhs;
      }
      lhs = parent.at(lhs);
      rhs = parent.at(rhs);
    }
  }
  int n_transfers{};
  for (Key lhs{"YOU"s}; lhs != lca; lhs = parent.at(lhs)) {
    n_transfers += 1;
  }
  for (Key rhs{"SAN"s}; rhs != lca; rhs = parent.at(rhs)) {
    n_transfers += 1;
  }
  return n_transfers - 2;
}

auto parse_input(const std::string path) {
  std::istringstream input{aoc::slurp_file(path)};
  auto edges{views::istream<Edge>(input) | ranges::to<std::vector>()};
  if (input.eof()) {
    return edges;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto edges{parse_input("/dev/stdin")};

  const auto parent{
      views::transform(edges, [](const Edge& e) { return std::pair{e.dst, e.src}; })
      | ranges::to<std::unordered_map>()
  };
  const auto part1{find_part1(parent)};
  const auto part2{find_part2(parent)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
