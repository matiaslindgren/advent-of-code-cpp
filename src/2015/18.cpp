import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Light : char {
  on = '#',
  off = '.',
  stuck = 'S',
};

std::istream& operator>>(std::istream& is, Light& light) {
  std::underlying_type_t<Light> ch;
  if (is >> ch) {
    switch (ch) {
      case std::to_underlying(Light::on):
      case std::to_underlying(Light::off):
      case std::to_underlying(Light::stuck):
        light = {ch};
        return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Light");
}

struct Grid2D {
  std::vector<Light> grid;
  std::size_t size;

  explicit Grid2D(const std::vector<Light>& lights)
      : grid{lights}, size{static_cast<std::size_t>(std::sqrt(lights.size()))} {
  }

  // TODO(llvm18) deducing this
  auto& get(const std::size_t row, const std::size_t col) {
    return grid.at(row * size + col);
  }
  const auto& get(const std::size_t row, const std::size_t col) const {
    return grid.at(row * size + col);
  }

  bool is_on(const Light l) const {
    return l == Light::on || l == Light::stuck;
  }
  bool is_on(const std::size_t row, const std::size_t col) const {
    return is_on(get(row, col));
  }

  int count_adjacent_on(const std::size_t row, const std::size_t col) const {
    int n{0};
    for (auto r{row - 1}; r <= row + 1; ++r) {
      for (auto c{col - 1}; c <= col + 1; ++c) {
        if (!(r == row && c == col) && is_on(r, c)) {
          ++n;
        }
      }
    }
    return n;
  }

  Grid2D pad() const {
    Grid2D padded{decltype(grid)((size + 2) * (size + 2), Light::off)};
    for (auto row{0uz}; row < size; ++row) {
      for (auto col{0uz}; col < size; ++col) {
        padded.get(row + 1, col + 1) = get(row, col);
      }
    }
    return padded;
  }

  Grid2D step() const {
    Grid2D next_grid{*this};
    for (auto row{1uz}; row < size - 1; ++row) {
      for (auto col{1uz}; col < size - 1; ++col) {
        if (get(row, col) == Light::stuck) {
          continue;
        }
        const auto on_count{count_adjacent_on(row, col)};
        if (is_on(row, col) && !(on_count == 2 || on_count == 3)) {
          next_grid.get(row, col) = Light::off;
        }
        if (!is_on(row, col) && on_count == 3) {
          next_grid.get(row, col) = Light::on;
        }
      }
    }
    return next_grid;
  }

  std::size_t count_on() const {
    return ranges::count_if(grid, [&](Light l) { return is_on(l); });
  }
};

Grid2D simulate(Grid2D grid, int steps) {
  for (int step{0}; step < steps; ++step) {
    grid = grid.step();
  }
  return grid;
}

int main() {
  aoc::init_io();

  const Grid2D input{views::istream<Light>(std::cin) | ranges::to<std::vector>()};
  Grid2D grid1{input.pad()};
  Grid2D grid2{grid1};
  {
    const auto n{grid2.size - 2};
    grid2.get(1, 1) = grid2.get(1, n) = grid2.get(n, 1) = grid2.get(n, n) = Light::stuck;
  }

  const auto part1{simulate(grid1, 100).count_on()};
  const auto part2{simulate(grid2, 100).count_on()};

  std::print("{} {}\n", part1, part2);

  return 0;
}
