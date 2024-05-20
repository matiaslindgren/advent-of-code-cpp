#include "aoc.hpp"
#include "std.hpp"

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

  auto&& get(this auto&& self, std::size_t row, std::size_t col) {
    return self.grid.at(row * self.size + col);
  }

  bool is_on(const Light l) const {
    return l == Light::on or l == Light::stuck;
  }
  bool is_on(const std::size_t row, const std::size_t col) const {
    return is_on(get(row, col));
  }

  int count_adjacent_on(const std::size_t row, const std::size_t col) const {
    int n{0};
    for (auto r{row - 1}; r <= row + 1; ++r) {
      for (auto c{col - 1}; c <= col + 1; ++c) {
        if (not(r == row and c == col) and is_on(r, c)) {
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
        if (is_on(row, col) and not(on_count == 2 or on_count == 3)) {
          next_grid.get(row, col) = Light::off;
        }
        if (not is_on(row, col) and on_count == 3) {
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
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const Grid2D grid0{views::istream<Light>(input) | ranges::to<std::vector>()};
  Grid2D grid1{grid0.pad()};
  Grid2D grid2{grid1};
  {
    const auto n{grid2.size - 2};
    grid2.get(1, 1) = grid2.get(1, n) = grid2.get(n, 1) = grid2.get(n, n) = Light::stuck;
  }

  const auto part1{simulate(grid1, 100).count_on()};
  const auto part2{simulate(grid2, 100).count_on()};

  std::println("{} {}", part1, part2);

  return 0;
}
