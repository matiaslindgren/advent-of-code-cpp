import std;

namespace ranges = std::ranges;
namespace views = std::views;

int get_digit(auto a, const auto k) {
  int d{};
  for (int i{}; i < k; ++i) {
    const auto [q, r]{std::div(a, 10)};
    d = r;
    a = q;
  }
  return d;
}

struct Result {
  long power{};
  std::size_t x{}, y{}, size{};
};

auto search(const int serial) {
  // https://en.wikipedia.org/wiki/Summed-area_table
  constexpr auto size{301uz};
  std::vector<long> sums(size * size, 0);
  std::mdspan grid(sums.data(), size, size);
  for (auto y{1uz}; y < size; ++y) {
    for (auto x{1uz}; x < size; ++x) {
      const auto power{get_digit((x + 10) * ((x + 10) * y + serial), 3) - 5};
      grid[x, y] = power + grid[x, y - 1] + grid[x - 1, y] - grid[x - 1, y - 1];
    }
  }
  std::vector<Result> results;
  for (auto n{1uz}; n < size; ++n) {
    auto& r{results.emplace_back(std::numeric_limits<long>::min(), 0, 0, n)};
    for (auto y{1uz}; y < size - n; ++y) {
      for (auto x{1uz}; x < size - n; ++x) {
        const auto ad{grid[x, y] + grid[x + n, y + n]};
        const auto bc{grid[x + n, y] + grid[x, y + n]};
        if (const auto power{ad - bc}; power > r.power) {
          r.power = power;
          r.x = x + 1;
          r.y = y + 1;
        }
      }
    }
  }
  return results;
}

int main() {
  std::ios::sync_with_stdio(false);

  int serial;
  std::cin >> serial;

  const auto results{search(serial)};

  const auto sq_3{results[2]};
  const auto part1{std::format("{},{}", sq_3.x, sq_3.y)};

  const auto sq_max{ranges::max_element(results, ranges::less{}, &Result::power)};
  const auto part2{std::format("{},{},{}", sq_max->x, sq_max->y, sq_max->size)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
