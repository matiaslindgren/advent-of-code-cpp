#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<unsigned>;

bool is_wall(const Vec2& p, const unsigned designer_num) {
  const auto wall_value{3 * p.x() + 2 * p.x() * p.y() + p.y() + (p * p).sum()};
  return std::popcount(wall_value + designer_num) % 2 != 0;
}

using Points = std::vector<Vec2>;
using Distances = std::unordered_map<Vec2, unsigned>;
constexpr auto max_distance{std::numeric_limits<unsigned>::max() - 1};

Distances find_shortest_paths(
    Vec2 begin,
    const Points& targets,
    unsigned designer_num,
    unsigned step_limit = max_distance
) {
  begin += Vec2(1, 1);
  step_limit += 1;

  Distances dist{{begin, 0}};
  const auto distance{[&dist](const Vec2& p) {
    return dist.contains(p) ? dist.at(p) : max_distance;
  }};

  for (const Vec2& target : targets) {
    std::unordered_set<Vec2> visited;
    for (auto q{dist | views::keys | ranges::to<std::deque>()}; not q.empty(); q.pop_front()) {
      Vec2 src{q.front()};
      if (src == target) {
        break;
      }
      if (auto [_, is_new]{visited.insert(src)}; is_new) {
        for (Vec2 dst : src.adjacent()) {
          if (dst.min() > 0 and not is_wall(dst - Vec2(1, 1), designer_num)
              and distance(src) + 1 < std::min(step_limit, distance(dst))) {
            dist[dst] = distance(src) + 1;
            q.push_back(dst);
          }
        }
      }
    }
  }

  return dist;
}

auto find_part1(unsigned designer_num) {
  const Vec2 end(32, 40);
  const auto distances{find_shortest_paths(Vec2(1, 1), Points{end}, designer_num)};
  return distances.at(end);
}

auto find_part2(unsigned designer_num, unsigned step_limit) {
  Points targets;
  for (Vec2 p; p.x() <= step_limit + 1; p.x() += 1) {
    for (p.y() = 0; p.sum() <= step_limit + 1; p.y() += 1) {
      targets.push_back(p + Vec2(1, 1));
    }
  }
  const auto distances{find_shortest_paths(Vec2(1, 1), targets, designer_num, step_limit)};
  return distances.size();
}

int main() {
  if (unsigned input{}; std::cin >> input) {
    const auto part1{find_part1(input)};
    const auto part2{find_part2(input, 50)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input, it should be a single unsigned integer");
}
