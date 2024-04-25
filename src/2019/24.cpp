#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = aoc::Vec3<int>;

struct Grid {
  std::unordered_set<Vec3> bugs;

  auto count_bugs(ranges::range auto&& points) const {
    return ranges::count_if(points, [&](Vec3 p) { return bugs.contains(p); });
  }

  bool in_grid(Vec3 p) const {
    return 0 <= p.y() and p.y() < 5 and 0 <= p.x() and p.x() < 5;
  }

  bool operator==(const Grid&) const = default;
};

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

std::vector<Vec3> get_adjacent(const Vec3& p) {
  return {
      p - Vec3(1, 0, 0),
      p - Vec3(0, 1, 0),
      p + Vec3(1, 0, 0),
      p + Vec3(0, 1, 0),
  };
}

auto find_part1(Grid g) {
  for (std::vector<Grid> seen; ranges::find(seen, g) == seen.end();) {
    seen.push_back(g);
    Grid g2{g};
    for (Vec3 bug : g.bugs) {
      const auto adjacent{get_adjacent(bug)};
      if (g.count_bugs(adjacent) != 1) {
        g2.bugs.erase(bug);
      }
      for (Vec3 p : adjacent) {
        if (g.in_grid(p) and not g.bugs.contains(p)) {
          if (auto n{g.count_bugs(get_adjacent(p))}; n == 1 or n == 2) {
            g2.bugs.insert(p);
          }
        }
      }
    }
    g = g2;
  }
  return sum(views::transform(views::iota(0, 25), [&g](long i) {
    auto [y, x]{std::ldiv(i, 5L)};
    return g.bugs.contains(Vec3(x, y, 0)) ? (1 << i) : 0;
  }));
}

auto get_adjacent_x(const Vec3& p) {
  std::vector<Vec3> adj;
  if (p.x() == 0) {
    adj.push_back(p + Vec3(1, 0, 0));
    adj.emplace_back(1, 2, p.z() + 1);
  } else if (p.x() == 4) {
    adj.push_back(p - Vec3(1, 0, 0));
    adj.emplace_back(3, 2, p.z() + 1);
  } else if (p.y() != 2) {
    adj.push_back(p - Vec3(1, 0, 0));
    adj.push_back(p + Vec3(1, 0, 0));
  } else if (p.x() == 1) {
    adj.push_back(p - Vec3(1, 0, 0));
    for (Vec3 q(0, 0, p.z() - 1); q.y() < 5; q.y() += 1) {
      adj.push_back(q);
    }
  } else if (p.x() == 3) {
    adj.push_back(p + Vec3(1, 0, 0));
    for (Vec3 q(4, 0, p.z() - 1); q.y() < 5; q.y() += 1) {
      adj.push_back(q);
    }
  } else {
    throw std::runtime_error(std::format("unhandled point {}", p));
  }
  return adj;
}

auto get_adjacent_recursive(const Vec3& p) {
  const auto adj_x{get_adjacent_x(p)};
  const auto adj_y{get_adjacent_x(Vec3(p.y(), p.x(), p.z()))};
  std::vector<Vec3> adj;
  adj.append_range(adj_x);
  adj.append_range(views::transform(adj_y, [](Vec3 p) { return Vec3(p.y(), p.x(), p.z()); }));
  return adj;
}

auto find_part2(Grid g) {
  for (int i{}; i < 200; ++i) {
    Grid g2{g};
    for (Vec3 bug : g.bugs) {
      const auto adjacent{get_adjacent_recursive(bug)};
      if (g.count_bugs(adjacent) != 1) {
        g2.bugs.erase(bug);
      }
      for (Vec3 p : adjacent) {
        if (p != Vec3(2, 2, 0) and not g.bugs.contains(p)) {
          if (auto n{g.count_bugs(get_adjacent_recursive(p))}; n == 1 or n == 2) {
            g2.bugs.insert(p);
          }
        }
      }
    }
    g = g2;
  }
  return g.bugs.size();
}

Grid parse_grid(std::string_view path) {
  Grid g;

  std::istringstream is{aoc::slurp_file(path)};
  {
    Vec3 pos;
    for (std::string line; std::getline(is, line) and line.size() == 5; pos.y() += 1) {
      pos.x() = 0;
      for (char ch : line) {
        if (ch == '#') {
          g.bugs.insert(pos);
        } else if (ch != '.') {
          throw std::runtime_error("every char must be # or .");
        }
        pos.x() += 1;
      }
    }
  }

  if (is.eof()) {
    return g;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const Grid grid{parse_grid("/dev/stdin")};

  const auto part1{find_part1(grid)};
  const auto part2{find_part2(grid)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
