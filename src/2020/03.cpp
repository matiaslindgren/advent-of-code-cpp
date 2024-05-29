#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Grid {
  std::vector<bool> trees;
  std::size_t width{};
};

auto slide_and_count(const Grid& g, const Vec2& d) {
  long n{};
  for (Vec2 p{}; p.y() < g.trees.size() / g.width; p += d) {
    n += int{g.trees.at(p.y() * g.width + p.x() % g.width)};
  }
  return n;
}

constexpr auto product{std::__bind_back(ranges::fold_left, 1L, std::multiplies{})};

auto search(const Grid g) {
  auto tree_counts{
      std::vector{Vec2(3, 1), Vec2(1, 1), Vec2(5, 1), Vec2(7, 1), Vec2(1, 2)}
      | views::transform([&g](const Vec2& d) { return slide_and_count(g, d); })
      | ranges::to<std::vector>()
  };
  return std::pair{tree_counts.front(), product(tree_counts)};
}

Grid parse_grid(std::string_view path) {
  Grid g;
  {
    std::istringstream is{aoc::slurp_file(path)};
    for (std::string line; std::getline(is, line) and not line.empty();) {
      if (g.width == 0) {
        g.width = line.size();
      } else if (line.size() != g.width) {
        throw std::runtime_error("every row must be of same width");
      }
      for (char ch : line) {
        if (ch != '#' and ch != '.') {
          throw std::runtime_error(std::format("input contains an unknown character {}", ch));
        }
        g.trees.push_back(ch == '#');
      }
    }
    if (not is.eof()) {
      throw std::runtime_error("input contains unknown characters after the grid");
    }
  }
  return g;
}

int main() {
  const auto [part1, part2]{search(parse_grid("/dev/stdin"))};
  std::println("{} {}", part1, part2);
  return 0;
}
