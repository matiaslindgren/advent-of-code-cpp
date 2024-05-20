#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

struct Grid {
  std::unordered_map<Vec2, int> heights;

  int height_at(Vec2 p) const {
    if (heights.contains(p)) {
      return heights.at(p);
    }
    return std::numeric_limits<int>::max();
  }

  Grid lower() const {
    Grid res{*this};
    ranges::for_each(res.heights | views::values, [](int& h) { h = std::max(0, h - 1); });
    return res;
  }
};

auto find_shortest_path(const Grid& grid, Vec2 src, Vec2 dst) {
  std::unordered_map<Vec2, int> dist;

  const auto get_dist{[&dist](const Vec2& pos) {
    if (not dist.contains(pos)) {
      dist[pos] = std::numeric_limits<int>::max();
    }
    return dist.at(pos);
  }};

  const auto push_min_dist_heap{[&get_dist](auto& q, const Vec2& pos) {
    q.push_back(pos);
    ranges::push_heap(q, ranges::greater{}, get_dist);
  }};

  const auto pop_min_dist_heap{[&get_dist](auto& q) {
    ranges::pop_heap(q, ranges::greater{}, get_dist);
    Vec2 pos{q.back()};
    q.pop_back();
    return pos;
  }};

  {
    std::vector q{{src}};
    dist[src] = 0;
    for (std::unordered_set<Vec2> visited; not q.empty();) {
      Vec2 p1{pop_min_dist_heap(q)};
      if (p1 == dst) {
        break;
      }
      if (auto [_, is_new]{visited.insert(p1)}; not is_new) {
        continue;
      }
      for (Vec2 p2 : p1.adjacent()) {
        const auto p1_height{grid.height_at(p1)};
        const auto p2_height{grid.height_at(p2)};
        if (p2_height - p1_height > 1) {
          continue;
        }
        const auto new_dist{aoc::saturating_add(get_dist(p1), int{p2_height > 0})};
        if (new_dist < get_dist(p2)) {
          dist[p2] = new_dist;
          push_min_dist_heap(q, p2);
        }
      }
    }
  }

  return get_dist(dst);
}

auto parse_grid(std::string_view path) {
  Grid grid;
  std::optional<Vec2> start, end;
  {
    std::istringstream is{aoc::slurp_file(path)};
    for (auto [row, p, width]{std::tuple{
             ""s,
             Vec2{},
             0uz,
         }};
         is >> row and not row.empty();
         p.y() += 1) {
      if (not width) {
        width = row.size();
      } else if (width != row.size()) {
        throw std::runtime_error("every row must be of same width");
      }
      p.x() = 0;
      for (char ch : row) {
        if (ch == 'S') {
          grid.heights[p] = 0;
          start = p;
        } else if (ch == 'E') {
          grid.heights[p] = 'z' - 'a' + 2;
          end = p;
        } else if ('a' <= ch and ch <= 'z') {
          grid.heights[p] = ch - 'a' + 1;
        } else {
          throw std::runtime_error(std::format("all input must be from [a-zSE], not '{}'", ch));
        }
        p.x() += 1;
      }
    }
    if (not start) {
      throw std::runtime_error("start S is missing");
    }
    if (not end) {
      throw std::runtime_error("end E is missing");
    }
    if (grid.heights.empty()) {
      throw std::runtime_error("empty input");
    }
    if (is.fail() and not is.eof()) {
      throw std::runtime_error("unknown error while parsing grid");
    }
  }
  return std::tuple{grid, *start, *end};
}

int main() {
  const auto [grid, start, end]{parse_grid("/dev/stdin")};

  const auto part1{find_shortest_path(grid, start, end)};
  const auto part2{find_shortest_path(grid.lower(), start, end)};

  std::println("{} {}", part1, part2);

  return 0;
}
