#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Point2D {
  using UInt = std::size_t;
  UInt x;
  UInt y;
  constexpr UInt wall_value() const noexcept {
    return x * x + 3 * x + 2 * x * y + y + y * y;
  }
  constexpr Point2D operator+(int a) const {
    return {x + a, y + a};
  }
  constexpr Point2D operator-(int a) const {
    return this->operator+(-a);
  }
  constexpr auto operator<=>(const Point2D& rhs) const = default;
};

template <>
struct std::hash<Point2D> {
  std::size_t operator()(const Point2D& p) const noexcept {
    // let's just hope x and y stay small
    constexpr auto half_width{std::numeric_limits<Point2D::UInt>::digits / 2};
    const auto lhs{p.x << half_width};
    const auto rhs{p.y & ((Point2D::UInt{1} << half_width) - 1)};
    return std::hash<std::size_t>{}(lhs | rhs);
  }
};

using Points = std::vector<Point2D>;
using Distances = std::unordered_map<Point2D, int>;

class Dijkstra {
 public:
  explicit Dijkstra(Point2D begin, Point2D end, int step_limit)
      : end{end}, step_limit{step_limit}, q{{begin}}, dist{{begin, 0}} {
  }

  explicit operator bool() const {
    return not q.empty() and not has_visited(end);
  }

  bool has_visited(const Point2D& p) const {
    return dist.contains(p);
  }

  int distance(const Point2D& p) const {
    if (const auto it{dist.find(p)}; it != dist.end()) {
      return it->second;
    }
    return std::numeric_limits<int>::max() - 1;
  }

  void visit(const Point2D& src, const Point2D& dst) {
    if (const auto new_d{distance(src) + 1}; new_d < std::min(step_limit, distance(dst))) {
      dist[dst] = new_d;
      push_min_dist_heap(dst);
    }
  }

  Point2D pop_next() {
    return pop_min_dist_heap();
  }

  Distances all_distances() const {
    return dist | views::transform([](const auto& item) -> std::pair<Point2D, int> {
             const auto& [k, v] = item;
             return {k - 1, v};
           })
           | ranges::to<Distances>();
  }

  void reset(Point2D new_end) {
    end = new_end;
    q = dist | views::keys | ranges::to<decltype(q)>();
    ranges::make_heap(q, ranges::greater{}, [=, this](const auto& p) { return distance(p); });
  }

 private:
  void push_min_dist_heap(const Point2D& new_p) {
    q.push_back(new_p);
    ranges::push_heap(q, ranges::greater{}, [=, this](const auto& p) { return distance(p); });
  }

  Point2D pop_min_dist_heap() {
    ranges::pop_heap(q, ranges::greater{}, [=, this](const auto& p) { return distance(p); });
    Point2D p{q.back()};
    q.pop_back();
    return p;
  }

  Point2D end;
  int step_limit;
  Points q;
  Distances dist;
};

constexpr bool is_wall(const Point2D& p, const int designer_num) {
  return std::popcount(p.wall_value() + designer_num) % 2 != 0;
}

Distances find_shortest_paths(
    const Point2D begin,
    const Points& targets,
    const std::size_t designer_num,
    const int step_limit = std::numeric_limits<int>::max() - 1
) {
  const std::array<std::pair<int, int>, 4> adj_deltas{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};
  Dijkstra search(begin + 1, begin + 1, step_limit + 1);
  for (const Point2D& target : targets) {
    search.reset(target + 1);
    while (search) {
      Point2D src{search.pop_next()};
      for (const auto& [dx, dy] : adj_deltas) {
        if (Point2D dst{src.x + dx, src.y + dy}; dst.x and dst.y and not search.has_visited(dst)
                                                 and not is_wall(dst - 1, designer_num)) {
          search.visit(src, dst);
        }
      }
    }
  }
  return search.all_distances();
}

auto find_part1(const int designer_num) {
  const Point2D end{31, 39};
  const auto distances{find_shortest_paths({1, 1}, {end}, designer_num)};
  return distances.at(end);
}

auto find_part2(const int designer_num, const int step_limit) {
  Points targets;
  for (std::size_t x{0}; x <= step_limit + 1; ++x) {
    for (std::size_t y{0}; x + y <= step_limit + 1; ++y) {
      targets.push_back({x, y});
    }
  }
  const auto distances{find_shortest_paths({1, 1}, targets, designer_num, step_limit)};
  return distances.size();
}

int main() {
  std::size_t input;
  std::cin >> input;

  const auto part1{find_part1(input)};
  const auto part2{find_part2(input, 50)};

  std::println("{} {}", part1, part2);

  return 0;
}
