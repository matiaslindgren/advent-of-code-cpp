import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto yx_range(auto y0, auto y1, auto x0, auto x1) {
  return my_std::views::cartesian_product(views::iota(y0, y1), views::iota(x0, x1));
}

struct Grid {
  using Point = std::pair<std::size_t, std::size_t>;
  using Cell = unsigned char;
  std::vector<Cell> cells;
  std::size_t width{};
  std::size_t height{};

  // TODO llvm18? deducing this
  constexpr Cell& get(auto y, auto x) {
    return cells[y * width + x];
  }
  constexpr const Cell& get(auto y, auto x) const {
    return cells[y * width + x];
  }

  constexpr auto iter_yx_with_padding(const std::size_t pad) const {
    return yx_range(pad, height - pad, pad, width - pad);
  }

  constexpr Grid pad() const {
    const auto w{width + 2};
    const auto h{height + 2};
    Grid out = {std::vector<Cell>(w * h, '.'), w, h};
    for (const auto& [y, x] : iter_yx_with_padding(0)) {
      out.get(y + 1, x + 1) = get(y, x);
    }
    return out;
  }

  constexpr bool is_symbol(const Cell c) const {
    return c != '.' and not std::isdigit(c);
  }

  constexpr bool is_gear(const Cell c) const {
    return c == '*';
  }

  constexpr int parse_int(auto y, auto x) const {
    int num{0};
    for (; std::isdigit(get(y, x)); ++x) {
      num = (num * 10) + (get(y, x) - '0');
    }
    return num;
  }

  constexpr auto adjacent_numbers(const Point& center) const {
    std::vector<Point> points;
    std::vector<int> numbers;
    for (const auto& [dy, dx] : yx_range(-1, 2, -1, 2)) {
      const auto [y, x] = center;
      if (auto y2{y + dy}, x2{x + dx}; std::isdigit(get(y2, x2))) {
        while (std::isdigit(get(y2, x2 - 1))) {
          --x2;
        }
        if (const Point adj{y2, x2}; ranges::find(points, adj) == points.end()) {
          points.push_back(adj);
          numbers.push_back(parse_int(y2, x2));
        }
      }
    }
    return numbers;
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
  if (is or is.eof()) {
    grid = g.pad();
    return is;
  }
  throw std::runtime_error("failed parsing Grid");
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

constexpr std::pair<int, int> search(const Grid& grid) {
  int part1{};
  int part2{};
  for (const auto& p : grid.iter_yx_with_padding(1uz)) {
    const auto& [y, x] = p;
    if (const auto cell{grid.get(y, x)}; grid.is_symbol(cell)) {
      if (const auto adj{grid.adjacent_numbers(p)}; not adj.empty()) {
        part1 += sum(adj);
        if (grid.is_gear(cell) and adj.size() == 2) {
          part2 += adj[0] * adj[1];
        }
      }
    }
  }
  return {part1, part2};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  Grid grid;
  input >> grid;

  const auto [part1, part2] = search(grid);
  std::print("{} {}\n", part1, part2);

  return 0;
}
