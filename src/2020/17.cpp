#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using Vec4 = ndvec::vec4<int>;

auto get_adjacent(Vec4 p, bool dim4) {
  std::vector<Vec4> adj;
  for (Vec4 d(-1, -1, -1, -int{dim4}); d.w() <= int{dim4}; d.w() += 1) {
    for (d.z() = -1; d.z() <= 1; d.z() += 1) {
      for (d.y() = -1; d.y() <= 1; d.y() += 1) {
        for (d.x() = -1; d.x() <= 1; d.x() += 1) {
          if (d != Vec4()) {
            adj.push_back(p + d);
          }
        }
      }
    }
  }
  return adj;
}

struct Grid {
  std::unordered_set<Vec4> active;
  std::size_t width{};

  [[nodiscard]]
  auto count_active(const auto& positions) const {
    return std::ranges::count_if(positions, [this](Vec4 p) { return active.contains(p); });
  }

  [[nodiscard]]
  Grid step(bool dim4) const {
    Grid out{*this};
    for (const Vec4& p : active) {
      const auto adjacent{get_adjacent(p, dim4)};
      if (auto n{count_active(adjacent)}; n != 2 and n != 3) {
        out.active.erase(p);
      }
      for (const Vec4& adj : adjacent) {
        if (not active.contains(adj)) {
          if (auto n{count_active(get_adjacent(adj, dim4))}; n == 3) {
            out.active.insert(adj);
          }
        }
      }
    }
    return out;
  }
};

auto simulate(Grid grid, auto n_cycles, bool dim4) {
  for (int c{}; c < n_cycles; ++c) {
    grid = grid.step(dim4);
  }
  return grid.active.size();
}

Grid parse_grid(std::string_view path) {
  Grid g;
  for (Vec4 pos; const std::string& line : aoc::slurp_lines(path)) {
    if (g.width == 0) {
      g.width = line.size();
    } else if (line.size() != g.width) {
      throw std::runtime_error("every row must be of same width");
    }
    pos.x() = 0;
    for (char ch : line) {
      if (ch == '#') {
        g.active.insert(pos);
      } else if (ch != '.') {
        throw std::runtime_error(std::format("invalid tile '{}', must be # or .", ch));
      }
      pos.x() += 1;
    }
    pos.y() += 1;
  }
  return g;
}

int main() {
  const Grid grid{parse_grid("/dev/stdin")};

  const auto part1{simulate(grid, 6, false)};
  const auto part2{simulate(grid, 6, true)};

  std::println("{} {}", part1, part2);

  return 0;
}
