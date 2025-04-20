#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto empty_cache_value{std::numeric_limits<std::size_t>::max()};

auto count_possible_cached(
    const auto& cache,
    const auto& towels,
    const auto& designs,
    const auto t,
    const auto d,
    const auto i
) {
  auto t_end{cache.extent(0) - 1};
  auto d_end{cache.extent(1) - 1};
  auto i_end{cache.extent(2) - 1};

  if (not(0 <= t and t <= t_end and 0 <= d and d <= d_end and 0 <= i and i <= i_end)) {
    return 0UZ;
  }

  if (std::size_t count{cache[t, d, i]}; count != empty_cache_value) {
    return count;
  }

  std::size_t count{};

  if (t != t_end and d != d_end) {
    const auto design{designs.at(d)};
    if (i == design.size()) {
      count = 1;
    } else {
      for (auto t2{0UZ}; t2 < towels.size(); ++t2) {
        const auto towel{towels.at(t2)};
        if (std::ranges::equal(towel, design | views::drop(i) | views::take(towel.size()))) {
          count += count_possible_cached(cache, towels, designs, t2, d, i + towel.size());
        }
      }
    }
  }

  return (cache[t, d, i] = count);
}

auto count_possible(const auto& towels, const auto& designs) {
  auto n_towels{towels.size() + 1};
  auto n_designs{designs.size() + 1};
  auto max_design_size{ranges::max(views::transform(designs, [](auto&& d) { return d.size(); }))};

  std::vector<std::size_t> cache_data(n_towels * n_designs * max_design_size, empty_cache_value);
  auto cache{std::mdspan(cache_data.data(), n_towels, n_designs, max_design_size)};

  std::size_t p1{}, p2{};
  for (int d{}; d < n_designs - 1; ++d) {
    const auto n{count_possible_cached(cache, towels, designs, 0, d, 0)};
    p1 += n > 0;
    p2 += n;
  }
  return std::pair{p1, p2};
}

auto parse_input(std::string_view path) {
  using std::operator""s;

  auto sections{
      views::split(aoc::slurp_file(path), "\n\n"s)
      | views::transform([](auto&& s) { return ranges::to<std::string>(s); })
      | ranges::to<std::vector>()
  };

  if (sections.size() == 2) {
    ranges::replace(sections[0], ',', ' ');
    std::istringstream is1{sections[0]};
    auto towels{views::istream<std::string>(is1) | ranges::to<std::vector>()};
    std::istringstream is2{sections[1]};
    auto designs{views::istream<std::string>(is2) | ranges::to<std::vector>()};
    if (is1.eof() and is2.eof() and not towels.empty() and not designs.empty()) {
      return std::pair{towels, designs};
    }
  }

  throw std::runtime_error("input must contain 2 sections separated by 2 newlines");
}

int main() {
  const auto [towels, designs]{parse_input("/dev/stdin")};
  const auto [part1, part2]{count_possible(towels, designs)};
  std::println("{} {}", part1, part2);
  return 0;
}
