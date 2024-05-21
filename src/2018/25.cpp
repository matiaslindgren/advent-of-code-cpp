#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec4 = ndvec::vec4<int>;
using Points = std::vector<Vec4>;
using aoc::skip;
using std::operator""s;

auto find_part1(const Points& points) {
  auto adjacent{
      views::transform(points, [](auto&& p) { return std::pair{p, std::vector<Vec4>{}}; })
      | ranges::to<std::unordered_map>()
  };

  for (auto&& [p1, p2] : my_std::views::cartesian_product(points, points)) {
    if (const auto d{p1.distance(p2)}; 0 < d and d <= 3) {
      adjacent.at(p1).push_back(p2);
    }
  }

  const auto ids{
      views::zip(points, views::iota(0UZ, points.size())) | ranges::to<std::unordered_map>()
  };

  constexpr auto root_sentinel{std::numeric_limits<std::size_t>::max()};
  std::vector<std::size_t> parents(points.size(), root_sentinel);

  const auto union_find_merge{[&](auto&& p1, auto&& p2) {
    auto root{ids.at(p1)};
    while (parents.at(root) != root_sentinel) {
      root = parents.at(root);
    }
    for (auto parent{ids.at(p2)}; parent != root_sentinel;) {
      const auto next{parents.at(parent)};
      if (parent != root) {
        parents[parent] = root;
      }
      parent = next;
    }
  }};

  for (auto&& [p1, adj] : adjacent) {
    for (auto&& p2 : adj) {
      union_find_merge(p1, p2);
    }
  }

  return ranges::count(parents, root_sentinel);
}

int main() {
  const auto points{aoc::parse_items<Vec4>("/dev/stdin", ',')};
  std::print("{}\n", find_part1(points));
  return 0;
}
