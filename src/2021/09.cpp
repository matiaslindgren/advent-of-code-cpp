#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};
constexpr auto product{std::__bind_back(ranges::fold_left, 1, std::multiplies{})};

struct Map {
  std::unordered_map<Vec2, int> heights;

  auto find_basins() const {
    return heights | views::keys | views::filter([this](Vec2 p1) {
             return ranges::all_of(p1.adjacent(), [p1, this](Vec2 p2) {
               const auto& h{this->heights};
               return not h.contains(p2) or h.at(p1) < h.at(p2);
             });
           });
  }

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

std::istream& operator>>(std::istream& is, Map& map) {
  Vec2 p;
  std::size_t width{};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (width and line.size() != width) {
      throw std::runtime_error("every row must be of same length");
    } else {
      width = line.size();
    }
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          map.heights[p] = ch - '0';
          p.x() += 1;
          continue;
      }
      throw std::runtime_error("all non-whitespace input must be digits");
    }
  }
  if (not is and not is.eof()) {
    throw std::runtime_error("failed parsing map");
  }
  return is;
}

int main() {
  std::ios::sync_with_stdio(false);
  Map m;
  std::cin >> m;
  const auto [part1, part2]{search_basins(m)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
