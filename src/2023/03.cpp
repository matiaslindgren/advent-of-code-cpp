import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr decltype(auto) yx_range(auto y0, auto y1, auto x0, auto x1) {
  const auto ny{y1 - y0};
  const auto nx{x1 - x0};
  // clang-format off
  const auto y_range{
    views::iota(decltype(y0){}, nx * ny) | views::transform([=](auto&& y) { return y0 + (y / ny); })
  };
  const auto x_range{
    views::iota(decltype(x0){}, nx * ny) | views::transform([=](auto&& x) { return x0 + (x % nx); })
  };
  // clang-format on
  return views::zip(y_range, x_range);
}

struct Grid {
  using Point = std::pair<std::size_t, std::size_t>;
  using Cell = unsigned char;
  std::vector<Cell> cells;
  std::size_t width;
  std::size_t height;

  // TODO llvm18? deducing this
  constexpr Cell& get(auto y, auto x) {
    return cells[y * width + x];
  }
  constexpr const Cell& get(auto y, auto x) const {
    return cells[y * width + x];
  }

  constexpr Grid pad() const {
    const auto w{width + 2};
    const auto h{height + 2};
    Grid out = {std::vector<Cell>(w * h, '.'), w, h};
    for (auto&& [y, x] : yx_range(0uz, height, 0uz, width)) {
      out.get(y + 1, x + 1) = get(y, x);
    }
    return out;
  }

  constexpr bool is_symbol(const Cell c) const {
    return c != '.' && !std::isdigit(c);
  }

  constexpr bool is_gear(const Cell c) const {
    return c == '*';
  }

  constexpr std::vector<Point> adjacent_numbers(auto y, auto x) const {
    std::vector<Point> points;
    for (auto&& [dy, dx] : yx_range(-1, 2, -1, 2)) {
      if (auto y2{y + dy}, x2{x + dx}; std::isdigit(get(y2, x2))) {
        while (std::isdigit(get(y2, x2 - 1))) {
          --x2;
        }
        if (const Point p{y2, x2}; ranges::find(points, p) == points.end()) {
          points.push_back(p);
        }
      }
    }
    return points;
  }

  constexpr int parse_int(auto y, auto x) const {
    int num{0};
    for (; std::isdigit(get(y, x)); ++x) {
      num = (num * 10) + (get(y, x) - '0');
    }
    return num;
  }
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  Grid g{};
  for (std::string line; std::getline(is, line); ++g.height) {
    g.width = line.size();
    for (std::stringstream ls{line}; ls;) {
      if (Grid::Cell c; ls >> c) {
        g.cells.push_back(c);
      }
    }
  }
  if (is || is.eof()) {
    grid = g;
    return is;
  }
  throw std::runtime_error("failed parsing Grid");
}

// TODO ranges::fold_left_first
constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus<int>())
};

constexpr int find_part1(const Grid& grid) {
  std::vector<Grid::Point> num_begin;
  for (auto&& [y, x] : yx_range(1uz, grid.height - 1, 1uz, grid.width - 1)) {
    if (grid.is_symbol(grid.get(y, x))) {
      num_begin.append_range(grid.adjacent_numbers(y, x));
    }
  }
  return sum(num_begin | views::transform([&grid](auto&& p) {
               const auto [y, x] = p;
               return grid.parse_int(y, x);
             }));
}

// TODO ranges::adjacent
constexpr decltype(auto) pairwise(auto&& r) {
  return views::zip(r, views::drop(r, 1)) | my_std::views::stride(2);
}

constexpr int find_part2(const Grid& grid) {
  std::vector<Grid::Point> num_begin;
  for (auto&& [y, x] : yx_range(1uz, grid.height - 1, 1uz, grid.width - 1)) {
    if (grid.is_gear(grid.get(y, x))) {
      if (const auto adj{grid.adjacent_numbers(y, x)}; adj.size() == 2) {
        num_begin.append_range(adj);
      }
    }
  }
  return sum(pairwise(num_begin) | views::transform([&grid](auto&& p) {
               const auto [p1, p2] = p;
               const auto [y1, x1] = p1;
               const auto [y2, x2] = p2;
               return grid.parse_int(y1, x1) * grid.parse_int(y2, x2);
             }));
}

int main() {
  std::ios_base::sync_with_stdio(false);

  Grid grid;
  std::cin >> grid;
  grid = grid.pad();

  const auto part1{find_part1(grid)};
  const auto part2{find_part2(grid)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
