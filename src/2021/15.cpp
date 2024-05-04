#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Grid {
  std::unordered_map<Vec2, int> risk;
  int width{}, height{};

  void expand(const std::size_t n) {
    for (Vec2 p(width, 0); p.y() < height; p.y() += 1) {
      for (p.x() = width; p.x() < n * width; p.x() += 1) {
        risk[p] = risk[p - Vec2(width, 0)] % 9 + 1;
      }
    }
    for (Vec2 p(0, height); p.x() < n * width; p.x() += 1) {
      for (p.y() = height; p.y() < n * height; p.y() += 1) {
        risk[p] = risk[p - Vec2(0, height)] % 9 + 1;
      }
    }
    width *= n;
    height *= n;
  }

  auto adjacent(Vec2 center) const {
    return center.adjacent() | views::filter([this](Vec2 p) { return this->risk.contains(p); });
  }
};

auto min_distance_to(const Grid& grid, Vec2 src, Vec2 dst) {
  // Dijkstra
  auto risk{
      grid.risk | views::keys
      | views::transform([](Vec2 p) { return std::pair{p, std::numeric_limits<int>::max()}; })
      | ranges::to<std::unordered_map>()
  };
  const auto get_risk{[&risk](const Vec2& p) { return risk.at(p); }};

  const auto push_min_risk_heap{[&get_risk](auto& q, Vec2 p) {
    q.push_back(p);
    ranges::push_heap(q, ranges::greater{}, get_risk);
  }};

  const auto pop_min_risk_heap{[&get_risk](auto& q) {
    ranges::pop_heap(q, ranges::greater{}, get_risk);
    Vec2 p{q.back()};
    q.pop_back();
    return p;
  }};

  std::vector<Vec2> q{{src}};
  risk.at(src) = 0;

  for (std::unordered_set<Vec2> visited; not q.empty();) {
    Vec2 p{pop_min_risk_heap(q)};
    if (p == dst) {
      break;
    }
    if (auto&& [_, unseen]{visited.insert(p)}; not unseen) {
      continue;
    }
    for (Vec2 adj : grid.adjacent(p)) {
      const int step_risk{aoc::saturating_add(get_risk(p), grid.risk.at(adj))};
      if (step_risk < get_risk(adj)) {
        risk.at(adj) = step_risk;
        push_min_risk_heap(q, adj);
      }
    }
  }

  return risk.at(dst);
}

Grid parse_grid(std::string_view path) {
  Grid g;
  {
    Vec2 p;
    std::istringstream is{aoc::slurp_file(path)};
    for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
      if (g.width and line.size() != g.width) {
        throw std::runtime_error("every row must be of same length");
      } else {
        g.width = line.size();
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
            g.risk[p] = ch - '0';
            p.x() += 1;
            continue;
        }
        throw std::runtime_error("all non-whitespace input must be digits");
      }
    }
    g.height = p.y();
    if (not g.width or not g.height or (not is and not is.eof())) {
      throw std::runtime_error("unknown error while parsing grid");
    }
  }
  return g;
}

int main() {
  auto grid{parse_grid("/dev/stdin")};
  grid.expand(5);

  const Vec2 corner(grid.width - 1, grid.height - 1);
  const auto part1{min_distance_to(grid, Vec2(), corner / Vec2(5, 5))};
  const auto part2{min_distance_to(grid, Vec2(), corner)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
