#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::is_digit;
using Vec2 = ndvec::vec2<int>;
using Cell = unsigned char;

bool is_symbol(Cell c) {
  return c != '.' and not is_digit(c);
}

bool is_gear(Cell c) {
  return c == '*';
}

struct Grid {
  std::unordered_map<Vec2, Cell> cells;

  [[nodiscard]]
  auto get(Vec2 p) const {
    return cells.contains(p) ? cells.at(p) : 0;
  }

  [[nodiscard]]
  int parse_number_at(Vec2 p) const {
    int num{};
    for (; is_digit(get(p)); p.x() += 1) {
      num = (num * 10) + (get(p) - '0');
    }
    return num;
  }

  [[nodiscard]]
  auto adjacent_numbers(Vec2 center) const {
    std::unordered_set<Vec2> visited;
    for (Vec2 d(-1, -1); d.y() <= 1; d.y() += 1) {
      for (d.x() = -1; d.x() <= 1; d.x() += 1) {
        if (Vec2 p{center + d}; is_digit(cells.at(p))) {
          while (is_digit(get(p - Vec2(1, 0)))) {
            p.x() -= 1;
          }
          visited.insert(p);
        }
      }
    }
    return views::transform(visited, [&](Vec2 p) { return parse_number_at(p); })
           | ranges::to<std::vector>();
  }
};

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};
constexpr auto product{std::bind_back(ranges::fold_left, 1, std::multiplies{})};

auto search(const Grid& grid) {
  int part1{};
  int part2{};
  for (auto&& [pos, cell] : grid.cells) {
    if (is_symbol(cell)) {
      if (const auto adj{grid.adjacent_numbers(pos)}; not adj.empty()) {
        part1 += sum(adj);
        if (is_gear(cell) and adj.size() == 2) {
          part2 += product(views::take(adj, 2));
        }
      }
    }
  }
  return std::pair{part1, part2};
}

Grid parse_grid(std::string_view path) {
  Grid grid;
  {
    Vec2 p;
    for (const auto& line : aoc::slurp_lines(path)) {
      p.x() = 0;
      for (char ch : line) {
        grid.cells[p] = ch;
        p.x() += 1;
      }
      p.y() += 1;
    }
  }
  return grid;
}

int main() {
  const auto [part1, part2]{search(parse_grid("/dev/stdin"))};
  std::println("{} {}", part1, part2);
  return 0;
}
