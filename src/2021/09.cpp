#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};
constexpr auto product{std::bind_back(ranges::fold_left, 1, std::multiplies{})};

struct Map {
  std::unordered_map<Vec2, int> heights;

  [[nodiscard]]
  auto find_basins() const {
    return heights | views::keys | views::filter([this](Vec2 p1) {
             return ranges::all_of(p1.adjacent(), [p1, this](Vec2 p2) {
               const auto& h{this->heights};
               return not h.contains(p2) or h.at(p1) < h.at(p2);
             });
           });
  }

  [[nodiscard]]
  auto find_basin_size(Vec2 center, auto& seen) const {
    if (auto [_, is_new]{seen.insert(center)}; not is_new) {
      return 0;
    }
    if (heights.at(center) == 9) {
      return 0;
    }
    return 1 + sum(views::transform(center.adjacent(), [&, this](Vec2 adj) {
             const auto& h{this->heights};
             if (h.contains(adj) and h.at(center) <= h.at(adj)) {
               return this->find_basin_size(adj, seen);
             }
             return 0;
           }));
  }
};

auto search_basins(const Map& m) {
  std::unordered_set<Vec2> seen;
  auto basins{
      m.find_basins() | views::transform([&m, &seen](Vec2 p) {
        return std::pair{
            m.heights.at(p) + 1,
            m.find_basin_size(p, seen),
        };
      })
      | ranges::to<std::vector>()
  };
  ranges::sort(basins, {}, [](auto&& basin) { return basin.second; });
  return std::pair{
      sum(views::elements<0>(basins)),
      product(views::elements<1>(basins) | views::drop(basins.size() - 3)),
  };
}

Map parse_map(std::string_view path) {
  Map map;
  for (Vec2 p; const std::string& line : aoc::slurp_lines(path)) {
    for (p.x() = 0; char ch : line) {
      if (not aoc::is_digit(ch)) {
        throw std::runtime_error("all non-whitespace input must be digits");
      }
      map.heights[p] = ch - '0';
      p.x() += 1;
    }
    p.y() += 1;
  }
  return map;
}

int main() {
  const Map m{parse_map("/dev/stdin")};
  const auto [part1, part2]{search_basins(m)};
  std::println("{} {}", part1, part2);
  return 0;
}
